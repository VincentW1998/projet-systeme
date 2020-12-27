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

static int cp2(char *tarSource1, char *tarTarget1,char *pathWithFile1, char *pathFileTarget1){
  //printf("\n| %s | %s | %s | %s |\n", tarSource1,tarTarget1,pathWithFile1,pathFileTarget1);
  /*for(int i = 0 ; i < strlen(pathFileTarget1);i++){
    if(pathFileTarget1[i]=='\0')
      printf("BOOM %d\n",i);
      }*/
  int n;
  cpOn = 1;
  if((n = checkEntete(tarTarget1, pathFileTarget1)) == 1) {
    cpOn = 0;
    return -1;
  }
  if((n = checkEntete(tarSource1, pathWithFile1)) == -1) {
    cpOn = 0;
    return -1;
  }
  cpOn = 0;
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
  memset(pathCd, '\0',1);
  char * tarSource = substringTar();
  char * pathWithFile = createPathFile(pathCp); // path du fichier src
  restorePosition();
  if(whichCd(pathCdTarget) == -1) {
    return -1;
  }
  memset(pathCd, '\0', 1);
  tarTarget = substringTar();
  pathFileTarget = createPathFile(pathCpTarget);
  cp2(tarSource,tarTarget,pathWithFile,pathFileTarget);
}
