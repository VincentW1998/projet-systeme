#include "myRmdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

// function delete (nbOption - 1) repository
int rmdirTar(int nbOption, char ** command) {
  storePosition();
  for(int i = 1; i < nbOption; i++) {
    if(deleteRepo(command[i]) == -1) {
      write(2, "erreur : rmdir\n", strlen("erreur : rmdir\n"));
    }
    restorePosition();
  }
  return 1;
}

// delete the repository
int deleteRepo(char * path) {
  //int fd, n;
  int n;
  char * pathRmdir = getLastToken(path);
  char * pathCd = pathWithoutLastToken(path, pathRmdir);

  if(whichCd(pathCd) == -1) // use cd function 
    return -1;


  if (TARPATH[0] == '\0') { // if we are not in tar file
    commandNoTar("rmdir", pathRmdir); // use exec rmdir 
    return 1;
  }

  char * tarName = substringTar(); // tar file name


  char * pathWithFolder = createPath(pathRmdir); // tarpath + pathRmdir

  // if the path is found in tar file, we delete it
  // else return -1
  rmdirOn = 1;
  if((n = checkEntete(tarName, pathWithFolder)) == -1) {
    rmdirOn = 0;
    return -1;
  }
  rmdirOn = 0;
  return 1;
}


