#include "myRmdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

char * empty = "rmdir: failed to remove : Directory not empty";
char * noFile = "rmdir: failed to remove : No such file or directory";

// function delete (nbOption - 1) repository
int rmdirTar(int nbOption, char ** command) {
  storePosition();
  for(int i = 1; i < nbOption; i++) {
    deleteRepo(command[i]);
    restorePosition();
  }
  return 1;
}

// delete the repository
int deleteRepo(char * path) {
  rmdirEmpty = 0; //for check the error
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
    return displayError(noFile);
  }
  rmdirOn = 0;
  if(rmdirEmpty)
    return displayError(empty);

  return 1;
}


