#include "myCp.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"
#include "monrm.h"
#include "myMkdir.h"

int chchecksum(int fichier){
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

int renommer (int fichier, char *s){
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

int chdroit(int fichier, char *s){
  //change les droits d'un fichier ou dossier tar
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.mode,s,8);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return chchecksum(fichier);
}

int chprop(int fichier, char *s){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.uname,s,32);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return chchecksum(fichier);
}

int chgroup(int fichier, char *s){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.gname,s,32);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return chchecksum(fichier);
}

int chgid(int fichier, char *s){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.gid,s,8);
  write(fichier,&entete,512);
  lseek(fichier, -512, SEEK_CUR);
  return chchecksum(fichier);
}

int chuid(int fichier, char *s){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  if(lect==0)
    return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }
  
  lseek(fichier, -512, SEEK_CUR);
  strncpy(entete.uid,s,8);
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
  
  /**if(check_checksum(&entete)==0){
    //printf("entete.name : %s\n",entete.name); 
    perror("Mauvaise entête. Je suis bloqué dans recherche !\n");
    return -1;
    }**/

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


int fin3(int fichier){
  debut3(fichier);
  while(1){
    int x = suivant3(fichier);
    if(x==2) break;
    else if(x==-1) return -1;
  }
  return 0;
}

static int modif(char *racine,char * suffixe,
		 char *chemin, char *nouveau){
  memset(nouveau,'\0',100);
  int i=0;int j=0;
  for(; i<strlen(suffixe)&&suffixe[i]!='\0'; i++){
    nouveau[i] = suffixe[i];
  }
  nouveau[i]='/';i++;
  while(racine[j]==chemin[j]){
    j++;
  }
  for(;j<strlen(chemin);i++){
    nouveau[i]=chemin[j];
    j++;
  }
  if(nouveau[i-1]=='/') nouveau[i-1]='\0';
  return 1;
}

static int modif2(char *racine, char *chemin, char *nouveau){
  memset(nouveau,'\0',100);
  int i=0;int j=0;
  while(racine[j]==chemin[j]){
    j++;
  }
  for(;j<strlen(chemin);i++){
    nouveau[i]=chemin[j];
    j++;
  }
  if(nouveau[i-1]=='/') nouveau[i-1]='\0';
  return 1;
}

/**
static void presence(char *fic1, char *supprimer){
  printf("supprimer : %s\n",supprimer);
  int fichier1 = open(fic1,O_RDWR);
  if(rechercher3(fichier1,1,supprimer)>=0){
    printf("Je suis présent.\n\n\n");
    rmfichier_tar(supprimer);
  }
  close (fichier1);
}
**/

static int estPresent(char *fic1, char *supprimer){
  int fichier1 = open(fic1,O_RDONLY);
  return (rechercher3(fichier1,1,supprimer)>=0);
}

static void init(char *a, char *b, char *p1, char *p2, char *p3){
  memset(a,'\0',100);memset(b,'\0',100);
  int i = 0;
  while(i<100 && p1[i]!='\0'){
    a[i]=p1[i]; i++;
  }
  int i2 = 0;
  while(i2<100 && p2[i2]!='\0'){
    b[i2]=p2[i2]; i2++;
  }
  a[i]='/';
  b[i2]='/';
  i++;i2++;
  int j=0;
  while ((i+j) < 100 && j<strlen(p3)
	 && p3[j]!='\0'){
    a[i+j] = p3[j];
    j++;
  }
}


static int copie(char *a, char *b){
  int i = 0;
  memset(b,'\0',100);
  for(; i < strlen(a);i++){
    b[i]=a[i];
  }
  // b[i]='/';
  return 1;
}

static int valide(char *fic1,char *path){
  int fichier1 = open(fic1,O_RDONLY);
  struct posix_header entete;
  if(rechercher3 (fichier1,1,path) != 1)
    return -1;//fichier source non présent
  read(fichier1,&entete,512);
  close(fichier1);
  if (entete.typeflag=='5') return -1;
  else return 1;
}

static int cp2(char *tarSource1, char *tarTarget1,
	       char *pathWithFile1, char *pathFileTarget1){
  int n;  
  cpOn = 1;
  printf("toujours là");
  if(estPresent(tarTarget1, pathFileTarget1)<=0){
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
  printf("Cas à résoudre : Écraser un fichier déjà présent.\n");
  return -1;
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
  for(; i < strlen(b) && a[i] == b[i] ; i ++){}
  for(int j = 0 ; j < strlen(a) ; j ++){
    c[j] = a[i+j];
  }
  return 1;
}

static int brutalcp(char *detect, char * morceau2, char *chemin, int fichier1, int fichier2){
  char *m = getLastToken(morceau2);
  char *m2 = pathWithoutLastToken(morceau2,m);
  struct posix_header et;
  char *ctaille = et.size;
  int taille; int nb;
  fin3(fichier1);
  int max = lseek(fichier1,0,SEEK_CUR);
  //printf("Nom : (%s)\n", morceau2);
  debut3(fichier1);
  fin3(fichier2);
  char tampon[512];
  char name[100];
  //On ajoute la base.
  //Ensuite, on ajoute le nom du repertoire dans lequel on souhaite écrire
  while(1){
    read(fichier1,&et,512);
    if(et.name[0]=='\0' || lseek(fichier1,0,SEEK_CUR) > max) break;
    ctaille = et.size;
    sscanf(ctaille,"%o",&taille);
    nb =((taille+512-1)/512);
    memset(name,'\0',100);
    strcat(name,m2);
    strcat(name,et.name);
    
   
    if(strncmp(et.name, detect, strlen(detect))==0
       && et.typeflag != '5'){
       printf("name : (%s)\n", name);
       write(fichier2,&et,512);
       lseek(fichier2,-512,SEEK_CUR);
       renommer(fichier2,name);
       lseek(fichier2,512,SEEK_CUR);
      for(int i = 0; i < nb; i++){
	read(fichier1,tampon,512);
	write(fichier2,tampon,512);
      }
    }
    else {
      lseek(fichier1,-512,SEEK_CUR);
      if(suivant3(fichier1)!=0) break;
    }
  }
  return 1;
}

int cprtar(char * path, char * target){
  char *tp= getTARPATH();
  char *ps= getPos();

  
  char * pathCp = getLastToken(path);
  char * pathCpTarget = getLastToken(target);
  char * pathCd = pathWithoutLastToken(path, pathCp);
  char * pathCdTarget = pathWithoutLastToken(target, pathCpTarget);


  char *mp= getTARPATH();
  char *ts= getPos();
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

  storePosition2(tp,ps);
  restorePosition();
  
  int n;
  //PARTIE CP_R
  int fichier1 = open(tarSource,O_RDONLY);
  int fichier2 = open(tarSource,O_RDWR);  
  if(fichier1<0 || fichier2<0){
    perror("Echec de l'ouverture du fichier");
    return -1;
  }
  
  char t2[100];
  fusion2(pathCdTarget,"",t2);
  
  if(rechercher3(fichier1,1,t2)!=1){
    return -1;//Dossier source non présent.
  }
  struct posix_header entete;
  //Le Dossier (contenant) est toujours avant dans l'arborescence.
  if(read(fichier1,&entete,512)<0){
    perror("Echec lecture du fichier");
  }

  if(entete.typeflag != '5') return -1;
  
  lseek(fichier1,-512,SEEK_CUR);
  suivant3(fichier1);
  char ntarget[100];
  fusion(target,pathCp,ntarget);
  navigate(pathCdTarget);
  char c[100];
  debut3(fichier1);
  while(1){
    read(fichier1,&entete,512);
    if(entete.name[0]=='\0') break;
    if((strncmp(entete.name,pathWithFile,strlen(pathWithFile)))==0
       && entete.typeflag=='5'){
	printf("Dossier : ");
	printf("entete.name (%s)\n",entete.name);
	fs(entete.name,pathCdTarget,c);
	storePosition();
	entete.name[(strlen(entete.name))-1]='\0';
	createRepo(entete.name);//pour tester
	restorePosition();
    }
    else {
      lseek(fichier1,-512,SEEK_CUR);
      if(suivant3(fichier1)!=0) break;
    }
  }  
  brutalcp(pathWithFile,target,path, fichier1, fichier2);
  return 1;
}



int cpTar(int noOption, char ** command) {
  storePosition();
  if(noOption > 3);
  if(strcmp(command[1],"-r")==0 && noOption >= 4){
    cprtar(command[2],command[3]);
  }
  else if(cpRepo(command[1], command[2]) == -1)
    write(2, "erreur : cp\n", strlen("erreur : mkdir\n"));
  restorePosition();
  return 1;
}
