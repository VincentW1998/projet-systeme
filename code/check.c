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
  if(filesize > BLOCKSIZE)
    lseek(fd, -512, SEEK_CUR);
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
    hasPosixHeader(fd); // for mkdir
    return -1;
  }
  if(strcmp(hd.name, path) == 0) {
    found = 1;

    int pos = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);
    if (countLinks(hd.name, fd) == 2 && rmdirOn) {
      lseek(fd, pos, SEEK_SET);
      hasRmdirOn(fd, filesize);
    }

  }
  return filesize;
}

int read_header_r(int fd, char *path){
  struct posix_header hd;
  unsigned int filesize = 0;
  size_t nb = read(fd, &hd, BLOCKSIZE);
  if(nb == -1) {
    perror("read");
    return -1;
  }
  found = 0;
  sscanf(hd.size, "%o", &filesize);
  if(hd.name[0] == '\0') {
    hasPosixHeader(fd); // for mkdir
    return -1;
  }
  int chm = strlen(path);
  if(strncmp(hd.name, path, chm) == 0) {
    found = 1;
    hasRm_r(fd,filesize);
  }
  //sscanf(hd.size, "%o", &filesize);
  return filesize;
}



int checkEntete_r(char * tarName, char * path) {
  int courant;
  int fd;
  int filesize = 0;
  fd = open(tarName, O_RDWR);
  int i = 0;
  while(1) {
    courant = lseek(fd,0,SEEK_CUR);
    // Utile en cas de suppression
    filesize = read_header_r(fd, path);
    if (filesize == -1) break;
    if (found) {
      i++;
      lseek(fd,courant,SEEK_SET);
      // On retourne à l'ancienne position
      /**read(fd,&entete,512);
      sscanf(entete.size, "%o", &filesize);
      printf("nom : %s\n",entete.name);
      next_header(fd,filesize);**/
      
    } 
    if(!found)
      next_header(fd, filesize);
  }
  close (fd);
  return (i>0);// Retourne s'il y a eu suppression
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


/*int decalage(int fd, int pos) {
  int fin = lseek(fd, 0, SEEK_END);
  int taille = fin - pos;
  lseek(fd, pos, SEEK_SET);
  return taille;
}*/

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
    return 1;
  }
  return -1;
}

int hasRm_r(int fd, int filesize){
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
