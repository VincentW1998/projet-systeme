#include "myCp.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

int cpTar(int noOption, char ** command) {
  storePosition();
    if(cpRepo(command[1], command[2]) == -1)
      write(2, "erreur : cp\n", strlen("erreur : mkdir\n"));
  restorePosition();
  return 1;
}

int cpRepo(char * path, char * target) {
  char * pathCd = subWithoutRepo(path);
  char * pathCdTarget = subWithoutRepo(target);

  if(whichCd(pathCd) == -1) {
    return -1;
  }
  char * tarSource = substringTar();

  char * pathCp = getLastToken(path);
  char * pathWithFile = createPathFile(pathCp); // path du fichier src

  restorePosition();

  if(whichCd(pathCdTarget) == -1) {
    return -1;
  }

  tarTarget = substringTar();

  char * pathCpTarget = getLastToken(target);
 // char * pathFileTarget = createPathFile(pathCpTarget);
  pathFileTarget = createPathFile(pathCpTarget);

/*  if(TARPATH[0] == '\0') {
    commandNoTar("cp", pathCp);
    return 1;
  } */
  //int fd, n;
  int n;
//  char * tarName = substringTar();
  
  /*fd = open(tarName, O_RDWR);
  if (fd < 0) {
    perror("open fichier Tar");
    return -1;
  }*/

  cpOn = 1;
  if((n = checkEntete(tarTarget, pathFileTarget)) == 1) {
    cpOn = 0;
    return -1;
  }

  if((n = checkEntete(tarSource, pathWithFile)) == -1) {
    cpOn = 0;
    return -1;
  }
  cpOn = 0;
  return 1;
}
