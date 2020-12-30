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
  char * pathCp = getLastToken(path);
  char * pathCpTarget = getLastToken(target);
  char * pathCd = pathWithoutLastToken(path, pathCp);
  char * pathCdTarget = pathWithoutLastToken(target, pathCpTarget);

  if(whichCd(pathCd) == -1) {
    return -1;
  }
  char * pwd = getcwd(NULL, 0);

  char *pathCp2 = malloc(strlen(pwd) + strlen(pathCp) + 2);
  memset(pathCp2, '\0', strlen(pwd) + strlen(pathCp) + 2);
  strcat(pathCp2, pwd);
  strcat(pathCp2, "/");
  strcat(pathCp2, pathCp);


  char * tarSource = substringTar();

  char * pathWithFile = createPathFile(pathCp); // path du fichier src

  restorePosition();

  if(whichCd(pathCdTarget) == -1) {
    return -1;
  }
  char * pwd2 = getcwd(NULL, 0);

  char *pathCpTarget2 = malloc(strlen(pwd2) + strlen(pathCpTarget) + 2);
  memset(pathCpTarget2, '\0', strlen(pwd2) + strlen(pathCpTarget) + 2);
  strcat(pathCpTarget2, pwd2);
  strcat(pathCpTarget2, "/");
  strcat(pathCpTarget2, pathCpTarget);
  
  if(TARPATH[0] == '\0') {
    commandNoTar_option("cp", pathCp2, pathCpTarget2);
    return 1;
  }

  memset(pathCd, '\0',1);
  memset(pathCdTarget, '\0', 1);

  tarTarget = substringTar();

  pathFileTarget = createPathFile(pathCpTarget);

  int n;

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
