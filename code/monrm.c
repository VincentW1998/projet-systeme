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
  char * pathCd = subWithoutRepo(path); // path cd

  if(whichCd(pathCd) == -1)
    return -1;

  char *pathRm = subWithRepo(path); // file a supprimer

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
  if((n = rmftar(tarName, pathWithFile)) == -1) { //check si le fichier existe
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
  char * pathCd = subWithoutRepo(path); // path cd

  if(whichCd(pathCd) == -1)
    return -1;

  char *pathRm = subWithRepo(path); // file a supprimer
  
  if(TARPATH[0] == '\0') { // si t'es pas dans un tar alors tu appelles exec
    commandNoTar_option("rm","-r", pathRm); // appel fonctions avec exec
    restorePosition(); // restorePosition
    return 1;
  }
  
  int x = (strlen(pathRm)+1);
  char pathRm2[x];
  memset(pathRm2,'\0',x);
  for(int i = 0; i < strlen(pathRm);i++){
    pathRm2[i]=pathRm[i];
  }
  pathRm2[strlen(pathRm)]='/';
  
  char * tarName = substringTar(); // recupere le nom du tar file

  // getcwd + tarpath + fichier a supprimer
  char * pathWithFile = createPathFile(pathRm); 
  rmOn = 1;
  if((n = checkEntete_r(tarName, pathWithFile)) == -1) { //check si le fichier existe
    rmOn = 0;
    restorePosition();
    return -1;
  }
  rmOn = 0;
  restorePosition();
  return 1;

  /** **/
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


/**void main(){
   int f = open("t.tar",O_RDWR);
  
   if(f<=0){
   perror("Erreur ouverture");
   return;
   }

   //rm_r_tar(f,"./","./t.tar","a/");
   //rmfichier_tar(f,"./","./t.tar","a/c");
   close(f);
   }**/
