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
    rmdirNoTar(pathRmdir);
    restorePosition();
  }

}


