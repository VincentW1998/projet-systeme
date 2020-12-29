#include "myCp.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"
#include "monrm.h"
#include "myMkdir.h"

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
  int i=0; int j=0;
  for(;i<strlen(a);i++){
    c[i]=a[i];
  }

  for(;j<strlen(b);j++){
    c[i+j]=b[j];
  }
}


int cprtar(char * path, char * target){
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
  //PARTIE CP_R
  int fichier1 = open(tarSource,O_RDONLY);  
  if(fichier1<0){
    perror("Echec de l'ouverture du fichier");
    return -1;
  }
  
  char t2[100];
  fusion2(pathCdTarget,"",t2);
  printf("test %s \n",t2);
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
  char ntarget[100];
  fusion(target,pathCp,ntarget);

  char * tp[100];
  copie(getTARPATH(),tp);
  char * ps [100];
  copie(getPos(),ps);
  
  navigate(pathCdTarget);
  createRepo(pathCpTarget);
  //Création du dossier
  storePosition2(tp,ps);
  restorePosition();
  
  if(suivant3(fichier1)!=0) return 1;
  while(1){
    copie(getTARPATH(),tp);
    copie(getPos(),ps);
    if(read(fichier1,&entete,512)<0){
     perror("Echec lecture du fichier");
    }
    if(entete.typeflag=='5'){
      if(strncmp(entete.name,t2,strlen(t2))==0){
	
	cprtar(path,entete.name);
	
      }
    }
    //Si c'est un dossier on applique la récursion.
    else {
      cpRepo(path,entete.name);
    }
    //Sinon, on copie les fichiers.
    if(lseek(fichier1,-512,SEEK_CUR)<0){
      perror("Echec de modification du décalage de fichier");
    }
    storePosition2(tp,ps);
    restorePosition();
    if(suivant3(fichier1)!=0) break;
  }
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
