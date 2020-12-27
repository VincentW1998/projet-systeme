#include "myRmdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

// function delete (nbOption - 1) repository
int rmdirTar(int nbOption, char ** command) {
  for(int i = 1; i < nbOption; i++) {
    if(deleteRepo(command[i]) == -1)
      return -1;
  }
  return 1;
}

// delete the repository
int deleteRepo(char * path) {
  //int fd, n;
  int n;
  storePosition();
  char * pathCd = subWithoutRepo(path); // path without the last repository

  if(whichCd(pathCd) == -1) // use cd function 
    return -1;

  char *pathRmdir = getLastToken(path); // path just with the last repository

  if (TARPATH[0] == '\0') { // if we are not in tar file
    commandNoTar("rmdir", pathRmdir); // use exec rmdir 
    restorePosition(); // restore the position
    return 1;
  }

  char * tarName = substringTar(); // tar file name

  /*fd = open(tarName, O_RDWR); // open the tar file
  if(fd < 0) {
    perror("open fichier Tar");
    return -1;
  }*/

  char * pathWithFolder = createPath(pathRmdir); // tarpath + pathRmdir

  // if the path is found in tar file, we delete it
  // else return -1
  rmdirOn = 1;
  if((n = checkEntete(tarName, pathWithFolder)) == -1) {
    rmdirOn = 0;
    restorePosition(); // restore position
    return -1;
  }
  rmdirOn = 0;
  restorePosition(); // restore position
  return 1;
}


