#include "gestionnaire.h"
//#include "util_tar.h"
#include "tar.h"
#include "monrm.h"
#include "storeRestore.h"
#include "check.h"
#include "myCd.h"


int contient(char *dossier,char *nom){
  DIR *d = opendir(dossier);
  struct dirent *ds = readdir(d);
  while (1){
    if(readdir(d)<=0) break;
    if(strcmp(ds->d_name,nom)==0) return 1;
  }
  return 0;
}

int rmfichier_tar(char * path){
  int fd, n;
  storePosition(); //store sa position
  char *pathRm = getLastToken(path); // file a supprimer
  char * pathCd = pathWithoutLastToken(path, pathRm);

  if(whichCd(pathCd) == -1)
    return -1;


  if(TARPATH[0] == '\0') { // si t'es pas dans un tar alors tu appelles exec
    commandNoTar("rm", pathRm); // appel fonctions avec exec
    restorePosition(); // restorePosition
    return 1;
  }

  char * tarName = substringTar(); // recupere le nom du tar file
  fd = open(tarName, O_RDWR);
  if(fd < 0) {
    perror("open fichier tar");
    return -1;
  }

// getcwd + tarpath + fichier a supprimer
  char * pathWithFile = createPathFile(pathRm); 
  rmOn = 1;
  if((n = checkEntete(tarName, pathWithFile)) == -1) { //check si le fichier existe
    rmOn = 0;
    restorePosition();
    return -1;
  }
  rmOn = 0;
  restorePosition();
  return 1;
}

int rm_r_tar (char * path){
  int n;
  storePosition(); //store sa position
  char *pathRm = getLastToken(path); // file a supprimer
  char * pathCd = pathWithoutLastToken(path, pathRm);
  if(whichCd(pathCd) == -1)
    return -1;

  
  if(TARPATH[0] == '\0') { // si t'es pas dans un tar alors tu appelles exec
    commandNoTar_option("rm","-r", pathRm); // appel fonctions avec exec
    restorePosition(); // restorePosition
    return 1;
  }
  
  char * tarName = substringTar(); // recupere le nom du tar file

  // getcwd + tarpath + fichier a supprimer
  char * pathWithFile = createPath(pathRm);
  if((n = checkEntete_r(tarName, pathWithFile)) == -1) { //check si le fichier existe
    restorePosition();
    return -1;
  }
  restorePosition();
  return 1;
}

int rmTar(int nbOption, char ** command) {
  int i = 1;
  int bool = 0;
  if(strcmp(command[1], "-r") == 0){
    i = 2;
    bool = 1;
  }
  for(; i < nbOption; i++) {
    if(bool) rm_r_tar(command[i]);
    else rmfichier_tar(command[i]);
  }
  return 1;
}


