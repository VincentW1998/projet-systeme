#include "myRmdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

int rmdirTar(int nbOption, char ** command) {
  for(int i = 1; i < nbOption; i++) {
    if(deleteRepo(command[i]) == -1)
      return -1;
  }
  return 1;
}

int deleteRepo(char * path) {
  storePosition();
  char * pathCd = subWithoutRepo(path);

  if(whichCd(pathCd) == -1)
    return -1;

  char *pathRmdir = subWithRepo(path);

  if (TARPATH[0] == '\0') {
    commandNoTar("rmdir", pathRmdir);
    restorePosition();
    return 1;
  }
  
  int fd, n;
  
  char * tarName = substringTar();

  fd = open(tarName, O_RDWR);
  if(fd < 0) {
    perror("open fichier Tar");
    return -1;
  }

  char * pathWithFolder = createPath(pathRmdir);

  if((n = checkEntete3(tarName, pathWithFolder)) == -1) {
    restorePosition();
    return -1;
  }
  restorePosition();
  return 1;
}


