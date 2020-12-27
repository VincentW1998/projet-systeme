#include "gestionnaire.h"
//#include "util_tar.h"
#include "tar.h"
#include "monrm.h"
#include "storeRestore.h"
#include "check.h"
#include "myCd.h"

static int debut3(int fichier){
  return lseek(fichier,0,SEEK_SET);
}

static int suivant3(int fichier){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);

  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture dans suivant ");
    return -1;
  }

  if(entete.name[0]=='\0'){
    //printf("cas ?\n");
    lseek(fichier,-512,SEEK_CUR);
    return 2;
  }
  
  char *ctaille = entete.size;
  int taille;
  sscanf(ctaille,"%o",&taille);
  int nb = (taille+512-1)/512;
  
  lseek(fichier,nb*512,SEEK_CUR);
  return 0;
}

static int recherche3(int fichier, int option, char *nom){
  //printf("Looping\n");
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture dans recherche");
    return -1;
  }

  if(entete.name[0]=='\0'){
    //printf("Je suis à la fin !!!");
    return -1;
  }
  
  if(check_checksum(&entete)==0){
    //printf("entete.name : %s\n",entete.name); 
    perror("Mauvaise entête. Je suis bloqué dans recherche !\n");
    return -1;
  }

  if(strcmp(entete.name,nom)==0){
    if(option == 1) lseek(fichier,-512,SEEK_CUR);
    else debut3(fichier);
    return 1;
  }

  lseek(fichier,-512,SEEK_CUR);

  switch(suivant3(fichier)){
  case 2:
    printf("over\n");
    return -1;
  case 0:
      return recherche3(fichier,option,nom);
  default :
    return -1;
  }
}

static int rechercher3 (int fichier, int option, char *nom){
  debut3(fichier);
  return recherche3(fichier, option, nom);
}

static int fin_fic3(int fichier){
  debut3(fichier);
  while(1){
    int x = suivant3(fichier);
    if(x==2) break;
    else if(x==-1) return -1;
  }
  return 0;
}

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
  char * pathWithFile = createPathFile(pathRm);
  
  fd = open(tarName, O_RDWR);

  if(fd < 0) {
    perror("open fichier tar");
    return -1;
  }

  if(rechercher3(fd,0,pathWithFile)<=0){
    return -1;
  }
  close(fd);

// getcwd + tarpath + fichier a supprimer
   
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
  int fd,n;
  storePosition(); //store sa position
  char * pathCd = subWithoutRepo(path); // path cd
  if(whichCd(pathCd) == -1)
    return -1;
  char *pathRm = subWithRepo(path); // file a supprimer
  int x = (strlen(pathRm)+5);
  char pathRm2[x];
  memset(pathRm2,'\0',x);
  int i = 0;
  while(i < strlen(pathRm) && pathRm[i]!='\0'){
    pathRm2[i]=pathRm[i];
    i++;
  }
  pathRm2[i]='/';
  if(TARPATH[0] == '\0') { // si t'es pas dans un tar alors tu appelles exec
    commandNoTar_option("rm","-r", pathRm); // appel fonctions avec exec
    restorePosition(); // restorePosition
    return 1;
  }
  char * tarName = substringTar(); // recupere le nom du tar file
  // getcwd + tarpath + fichier a supprimer
  char * pathWithFile = createPathFile(pathRm2); 
  rmOn = 1;

  fd = open(tarName, O_RDWR);
  
  if(fd < 0) {
    perror("open fichier tar");
    return -1;
  }

  if(rechercher3(fd,0,pathWithFile)<=0){
    return -1;
  }
  close(fd);
  
  if((n = checkEntete_r(tarName, pathWithFile)) == -1) {
    //check si le fichier existe
    rmOn = 0;
    restorePosition();
    return -1;
  }
  rmOn = 0;
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
