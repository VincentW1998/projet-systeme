#include "tar.h"
#include "check.h"
#include "myLs.h"


// saut vers le prochain header
void next_header(int fd, unsigned int filesize) {
  unsigned int nblocks = (filesize + BLOCKSIZE - 1) >> BLOCKBITS;
  for(int i = 0; i < nblocks; i++) {
    size_t nb = read(fd, buf, BLOCKSIZE);
    if(nb == -1) {
      perror("read");
    }
  }
}


int read_header(int fd, char *path) {
  unsigned int filesize = 0;
  struct posix_header hd;
  size_t nb = read(fd, &hd, BLOCKSIZE);
  if(nb == -1) {
    perror("read");
    return -1;
  }
  found = 0;
  sscanf(hd.size, "%o", &filesize);
  if(hd.name[0] == '\0') {
    if(cpOn) {
      endFile = lseek(fd, -512, SEEK_CUR);
      return -1;
    }
    hasPosixHeader(fd); // for mkdir
    return -1;
  }
  if(strcmp(hd.name, path) == 0) {
    found = 1;

    if(rmOn)
      hasRmdirOn(fd, filesize);

    int pos = lseek(fd, 0, SEEK_CUR);
    if(rmdirOn && hd.typeflag == '5') {
      lseek(fd, 0, SEEK_SET);
      // if we use rm/rmdir and the file is empty
      if (countLinks(hd.name, fd) == 2) {
        lseek(fd, pos, SEEK_SET);
        hasRmdirOn(fd, filesize);
      }
      else
        rmdirEmpty = 1;
    }
    if(cpOn) {
      hasCpOn(fd, filesize);
    }

  }
  return filesize;
}

int checkEntete_r(char * tarName, char * path) {
  int fd;
  fd = open(tarName, O_RDWR);
  char * b = "tamponForRm"; // name for file tampon
    int ft = open(b,O_CREAT|O_RDWR,S_IRWXU);
  //Partie II : copie dans le fichier tampon
  char tampon[BLOCKSIZE];

  while(read(fd,&tampon,BLOCKSIZE)>0){
    write(ft,&tampon,BLOCKSIZE);
  }

  ftruncate(fd,0);
  int taille,nb,j;
  //Partie III : Replacement
  struct posix_header entete;
  lseek(fd,0,SEEK_SET);
  lseek(ft,0,SEEK_SET);
  int n;
  while((n = read(ft,&entete,BLOCKSIZE))>0){
    j=0;
    sscanf(entete.size,"%o",&taille);
    nb=((taille+512-1)/512);
    if(strcmp(entete.name, path)!=0){
      write(fd,&entete,512);
      while(j<nb){
	read(ft,&tampon,BLOCKSIZE);
	write(fd,&tampon,BLOCKSIZE);
	j++;
      }
    }
    else lseek(ft,nb*512,SEEK_CUR);
  }
  char tamp[1024];
  memset(&tamp,'\0',1024);
  write(fd,tamp,1024);
  close(ft);
  close(fd);
  unlink(b);
  return 1;
}


int checkEntete(char * tarName, char * path) {
  int fd;
  int filesize = 0;
  fd = open(tarName, O_RDWR);

  if(fd<0){
    perror("Erreur ouverture fichier tar");
    return -1;
  }
  
  while(1) {
    filesize = read_header(fd, path);
    if (filesize == -1) break;
    if (found) {
      close(fd);
      return 1;
    }
    next_header(fd, filesize);
  }
  close (fd);
  return -1;
}


int fin (int fd, int pos) {
  int fin = lseek(fd, 0, SEEK_END);
  lseek(fd, pos, SEEK_SET);
  return fin;
}

// check if we have a posixHeader created by mkdir command
int hasPosixHeader(int fd){
  if(newHd.name[0] != '\0') {
    lseek(fd, -512, SEEK_CUR);
    char blockEnd[BLOCKSIZE];
    memset(blockEnd, '\0', BLOCKSIZE);
    write(fd, &newHd, BLOCKSIZE);
    write(fd, blockEnd, BLOCKSIZE);
    memset(&newHd, '\0', BLOCKSIZE);
    return 1;
  }
  return -1;
}

/* delete the directory */
int hasRmdirOn(int fd, int filesize) {
  char tampon[BLOCKSIZE];
  int n = lseek(fd, 0, SEEK_CUR);
  int fin2 = fin(fd, n);
  int accu = -512; // accumulateur add 512 each read/write
  int nb = (filesize + 512 -1)/512; // nb of blocks of the file
  lseek(fd, n + (nb * 512), SEEK_SET); // go to the end of the file
  while( lseek(fd, 0, SEEK_CUR)<fin2) { // while we are not at the end
    read(fd, &tampon, BLOCKSIZE); // read
    pwrite(fd, &tampon, BLOCKSIZE, n + accu); // write
    accu +=512;
  }
  return 0;
}

/* copy the file */
int hasCpOn(int fd, int filesize) {
  char tampon[BLOCKSIZE]; // tampon pour recuper le contenu
  char blockEnd[BLOCKSIZE]; // block vide
  lseek(fd, -512, SEEK_CUR); 
  read(fd, &newHd, BLOCKSIZE); // read file source
  memset(newHd.name, '\0', 100);
  strncpy(newHd.name, pathFileTarget, 100);
  set_checksum(&newHd);
  check_checksum(&newHd);
  int nb = (filesize + 512 -1) / 512;
  int fd2 = open(tarTarget, O_RDWR);
  pwrite(fd2, &newHd, BLOCKSIZE, endFile);
  int accu = 512;
  for (int i = 0; i < nb; i++) {
    read(fd, &tampon, BLOCKSIZE);
    pwrite(fd2, &tampon, BLOCKSIZE, endFile + accu);
    accu += 512;
  }

  memset(blockEnd, '\0', BLOCKSIZE);
  pwrite(fd2, blockEnd, BLOCKSIZE, endFile + accu);
  memset(&newHd, '\0', BLOCKSIZE); // vide le posix_header 
  return 0;
}
