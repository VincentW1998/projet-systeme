#include "myCp.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"
#include "monrm.h"
#include "myMkdir.h"

static int chchecksum(int fichier){
  // Modifie le checksum de l'entête courante.
  // Est utilisé à chaque fois qu'on modifie l'entête.
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  set_checksum (&entete);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return check_checksum(&entete);
}

static int renommer (int fichier, char *s){
  /*change le nom à la position courante
   *d'un fichier ou dossier dans un tar
   */
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture dans renommer");
    return -1;
  }
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.name,s,100);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return chchecksum(fichier);
}

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
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture dans recherche");
    return -1;
  }

  if(entete.name[0]=='\0'){
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


int fin3(int fichier){
  debut3(fichier);
  while(1){
    int x = suivant3(fichier);
    if(x==2) break;
    else if(x==-1) return -1;
  }
  return 0;
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
  char * pathWithFile = createPathFile(pathCp);
  // path du fichier src
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

static void fusion(char *a, char *b, char *c){
  memset(c,'\0',100);
  int i=0; int j=0;
  for(;i<strlen(a);i++){
    c[i]=a[i];
  }
  c[i]='/';
  i++;

  for(;j<strlen(b);j++){
    c[i+j]=b[j];
  }
}

static void fusion2(char *a, char *b, char *c){
  memset(c,'\0',100);
  int i=0; int j=0;
  for(;i<strlen(a);i++){
    c[i]=a[i];
  }

  for(;j<strlen(b);j++){
    c[i+j]=b[j];
  }
}

static int fs(char *a, char *b, char *c){
  int i =0;
  memset(c,'\0',100);
  while(a[i] == b[i] && i<strlen(a) && i < strlen(b)){
    i++;
  }
  for(int j = 0 ; j < strlen(a) ; j ++){
    c[j] = a[i+j];
  }
  return 1;
}

static void fusion3(char *a, char * b, char * retour){
  int i = 0; int j= 0;
  memset(retour,'\0',100);
  for(;i<strlen(a);i++){
    retour[i]=a[i];
  }
  for(;j<strlen(b);j++){
    retour[i+j]=b[j];
  }
}

int cprtar(char * path, char * target){

  if(path == target) return -1;
  
  char * pathCp = getLastToken(path);
  char * pathCpTarget = getLastToken(target);
  char * pathCd = pathWithoutLastToken(path, pathCp);
  char * pathCdTarget = pathWithoutLastToken(target, pathCpTarget);
  
  if(whichCd(pathCd) == -1) return -1;
  char * tarSource = substringTar();
  char * pathWithFile = createPath(pathCp);
  
  restorePosition();

  if(whichCd(pathCdTarget) == -1) return -1;
  
  
  tarTarget = substringTar();
  pathFileTarget = createPath(pathCpTarget);

  int t = open(tarSource,O_RDONLY);
  
  close(t);
  int fichier1 = open(tarSource,O_RDONLY);
  int fichier2 = open(tarTarget,O_RDWR);  
  if(fichier1<0 || fichier2<0){
    perror("Echec de l'ouverture du fichier");
    return -1;
  }
  
  char t2[100];
  fusion2(pathCdTarget,"",t2);
  if(rechercher3(fichier1,1,pathWithFile)!=1){
    return -1;//Dossier source non présent.
  }
  struct posix_header entete;
  if(read(fichier1,&entete,512)<0) perror("Echec lecture du fichier");
  if(entete.typeflag != '5') return -1;
  
  lseek(fichier1,-512,SEEK_CUR);
  suivant3(fichier1);
  char ntarget[100];
  fusion(target,pathCp,ntarget);  
  char c[100];

  if(recherche3(fichier2,0,pathFileTarget)!=1){
    fin3(fichier2);
    strcpy(entete.name,pathFileTarget);
    write(fichier2,&entete,512);
    lseek(fichier2,-512,SEEK_CUR);
    chchecksum(fichier2);
    lseek(fichier2,512,SEEK_CUR);
    fin3(fichier2);
  }
  
  fin3(fichier1);
  int f = lseek(fichier1,0,SEEK_CUR);
  debut3(fichier1);
  fin3(fichier2);
  char tnom[100];
  char pcd[100];
  memset(pcd,'\0',100);
  strcat(pcd, pathCd);
  char *ctaille = entete.size;
  int taille;
  sscanf(ctaille,"%o",&taille);
  int nb = ((taille+512-1)/512);
  char tampon[512];
  while(1){
    read(fichier1,&entete,512);
    if(entete.name[0]=='\0' || lseek(fichier1,0,SEEK_CUR)>f) break;
    if((strncmp(entete.name,pathWithFile,strlen(pathWithFile)))==0
       && entete.typeflag=='5'){
      if(strcmp(path,pathCp)==0) fs(entete.name,pcd,c);
      else fs(entete.name,pcd,c);
      fusion3(pathFileTarget,c,tnom);
      if(rechercher3(fichier2,0,tnom) != 1){
	fin3(fichier2);
	write(fichier2,&entete,512);
	lseek(fichier2,-512,SEEK_CUR);
	renommer(fichier2,tnom);
	lseek(fichier2,512,SEEK_CUR);
      }
    }
    else {
      lseek(fichier1,-512,SEEK_CUR);
      if(suivant3(fichier1)!=0) break;
    }
  }
  debut3(fichier1);
  fin3(fichier2);
  while(1){
    read(fichier1,&entete,512);
    if(entete.name[0]=='\0' || lseek(fichier1,0,SEEK_CUR)>f) break;
    if((strncmp(entete.name,pathWithFile,strlen(pathWithFile)))==0
       && entete.typeflag!='5'){
      if(strcmp(path,pathCp)==0) fs(entete.name,pathCd,c);
      else fs(entete.name,pathCp,c);
      fusion3(pathFileTarget,c,tnom);
      if(rechercher3(fichier2,0,tnom) != 1){
	fin3(fichier2);
	write(fichier2,&entete,512);
	lseek(fichier2,-512,SEEK_CUR);
	renommer(fichier2,tnom);
	lseek(fichier2,512,SEEK_CUR);
	ctaille=entete.size;
	sscanf(ctaille,"%o",&taille);
	nb = ((taille+512-1)/512);
	for(int i = 0 ; i < nb ; i ++){
	  read(fichier1,&tampon,512);
	  write(fichier2,&tampon,512);
	}
      }
    }
    else {
      lseek(fichier1,-512,SEEK_CUR);
      if(suivant3(fichier1)!=0) break;
    }
  }
  return 1;
}

int cpTar(int noOption, char ** command) {
  storePosition();
  if(strcmp(command[1],"-r")==0 && noOption >= 4){
    cprtar(command[2],command[3]);
  }
  else if(cpRepo(command[1], command[2]) == -1)
    write(2, "erreur : cp\n", strlen("erreur : mkdir\n"));
  restorePosition();
  return 1;
}
