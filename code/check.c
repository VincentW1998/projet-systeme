#include "tar.h"
#include "check.h"

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
  if(strcmp(hd.name, path) == 0) {
    found = 1;
    if(rmdirOn)
      hasRmdirOn(fd); // for rmdir
    if(rmOn)
      hasRmOn(fd, filesize);
  }
  //sscanf(hd.size, "%o", &filesize);
  return filesize;
}


int checkEntete(char * tarName, char * path) {
  int fd;
  int filesize = 0;
  fd = open(tarName, O_RDWR);
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


int decalage(int fd, int pos) {
  int fin = lseek(fd, 0, SEEK_END);
  int taille = fin - pos;
  lseek(fd, pos, SEEK_SET);
  return taille;
}

int fin (int fd, int pos) {
  int fin = lseek(fd, 0, SEEK_END);
  int taille = fin - pos;
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

int hasRmdirOn(int fd) {
  int n = lseek(fd, 0, SEEK_CUR); // position actuelle
  int taille = decalage(fd, n); // trouve le decalage
  char buf[taille];
  read(fd, &buf, taille); // lecture du reste du fichier
  lseek(fd, n - 512, SEEK_SET); // go to initial position minus 512
  found = 1;
  write(fd, buf, taille); // write over 
  return 0;
}

// si tu utilises rm 
int hasRmOn(int fd, int filesize) {
  char tampon[BLOCKSIZE];
  int n = lseek(fd, 0, SEEK_CUR);
  int fin2 = fin(fd, n);
  int accu = -512;
  int nb = (filesize + 512 -1)/512;
  lseek(fd, n + (nb * 512), SEEK_SET);
  
  while( lseek(fd, 0, SEEK_CUR)<fin2) {
    read(fd, &tampon, BLOCKSIZE);
    pwrite(fd, &tampon, BLOCKSIZE, n + accu);
    accu +=512;
  }
  return 0;
}
