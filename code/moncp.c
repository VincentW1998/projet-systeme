#include "util_tar.h"
#include "monrm.c"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

int clear(int fichier, char *dos, char *archive ,char *c){
  /*La fonction clear sert à supprimer 
   *le fichier qui doit être écrasé 
   *par la copie du nouveau fichier, si il est présent 
   *et retourne le nouveau descripteur de fichier.
   *Sinon, retourne l'ancien descripteur de fichier.
   */
  if(rechercher(fichier,1,c)){
    struct posix_header entete;
    ssize_t lect =read(fichier,&entete,512);
    if(entete.typeflag != '5'){
      return rmfichier_tar(fichier,dos,archive,c);
    }
    else return fichier;
  }
  else return fichier;
    
}

int cpfichier_intratar(int fichier,char *fic,char *c){
  //Copie à la fin du tar un fichier déjà présent.
  fin(fichier);
  off_t decalage = lseek(fichier,0,SEEK_CUR);
  
  rechercher(fichier,1,fic);
  
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);
  
  strncpy(entete.name,c,100);
  set_checksum(&entete);
  
  pwrite(fichier,&entete,512,decalage);
  
  decalage+=512;
  char *tampon [512];
  
  char *ctaille = entete.size;
  int taille;
  int sc = sscanf(ctaille,"%o",&taille);
  int nb = ((taille+512-1)/512);

  for (int i = 0; i<nb;i++){
    read(fichier,&tampon,512);
    pwrite(fichier,tampon,512,decalage);
    decalage+=512;
  }
  char *tnull[1024];
  //correspond aux deux blocs finaux de taille 512 de l'archive tar.
  memset(tnull,(int)'\0',1024);
  pwrite(fichier,tnull,1024,decalage);
  return 0;
}


int cpfichier_intertar
(int fichier1, int fichier2, char *dossierarchive,char *cheminarchive, char *cible, char * dest){
  rechercher(fichier1,1,cible);
  int test = fichier2;
  fichier2 = clear(fichier2,dossierarchive, cheminarchive,dest);
  
  if(test == fichier2){
    rechercher(fichier2,1,dest);
    struct posix_header tete;
    ssize_t lect = read(fichier2,&tete,512);
    if(tete.typeflag == '5'){
      return fichier2;
    }
  }
  
  fin(fichier2);
  
  struct posix_header entete;
  ssize_t lect = read(fichier1,&entete,512);
  write(fichier2,&entete,512);
  if(check_checksum(&entete)==0){
    perror("erreur sur le checksum");
  }
  
  lseek(fichier2,-512,SEEK_CUR);
  renommer(fichier2,dest);
  check_checksum(&entete);
  
  lseek(fichier2,512,SEEK_CUR);
  char *tampon [512];
  char *ctaille = entete.size;
  int taille;
  int sc = sscanf(ctaille,"%o",&taille);
  int nb = ((taille+512-1)/512);

  for (int i = 0; i<nb;i++){
    read(fichier1,tampon,512);
    write(fichier2,tampon,512);
  }
  char *tnull[1024];
  memset(tnull,(int)'\0',1024);
  write(fichier2,tnull,1024);
  fin(fichier2);
  return fichier2;
}

int cp_r_intertar(int fichier1, int fichier2, char *dosarc2, char *arc2, char *fic, char *c){
  //copie récursive dossier (cp -r)
  struct posix_header entete;
  if(rechercher(fichier1,0,fic)==1){
    int chm = strlen(fic);
    char *ctaille = entete.size;
    int taille;
    int sc = sscanf(ctaille,"%o",&taille);
    int nb = ((taille+512-1)/512);
    char tab[100];
    memset(tab,'\0',100);
    int i=0;
    while(i<100 && i<strlen(c)){
      tab[i]=c[i];
      i++;
    }
    int c2;
    int r1;
    debut(fichier1);
    debut(fichier2);
    while(read(fichier1,&entete,512)>0){
      ctaille = entete.size;
      taille;
      sc = sscanf(ctaille,"%o",&taille);
      nb = ((taille+512-1)/512);
      if(strncmp(entete.name,fic,chm)!=0){
	lseek(fichier1,nb*512,SEEK_CUR);
      }
      else {
	r1=i;
	c2=0;
	while((i+c2)<100 && c2<strlen(entete.name)){
	  tab[r1]=entete.name[c2];
	  c2++;
	  r1++;
	}
	int parcours = lseek(fichier1,0,SEEK_CUR);
	fichier2 = cpfichier_intertar(fichier1, fichier2, dosarc2, arc2,entete.name, tab);
	lseek(fichier1,parcours,SEEK_SET);   
	lseek(fichier1,nb*512,SEEK_CUR);
	memset(tab,'\0',100);
	memset(&entete,'\0',512);
	i=0;
	while(i<100 && i<strlen(c)){
	  tab[i]=c[i];
	  i++;
	}
	
      }
    }
    return 0;
  }
  return 0;
}

int cp_r_intratar(){
  return 0;
}

int main(){
  int f = open("a.tar",O_RDWR);
  int g = open("b.tar",O_RDONLY);
  if(f<=0){
    perror("( f )");
  }

  if (g<=0){
    perror("( g )");
  }
  
  cp_r_intertar(g,f,"./","./a.tar","b/","a/");
}
