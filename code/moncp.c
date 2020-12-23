#include "util_tar.h"
#include "monrm.c"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

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

int clear(int fichier, char *dos, char *archive ,char *c){
  if(rechercher(fichier,1,c)){
    struct posix_header entete;
    ssize_t lect =read(fichier,&entete,512);
    if(entete.typeflag!='5'){
      return rmfichier_tar(fichier,dos,archive,c);
    }
  }
  else return -1;
}

int cpfichier_intertar(int fichier1, int fichier2, char *dosarc2, char *arc2, char *fic, char *c){
  rechercher(fichier1,1,fic);
  fichier2 = clear(fichier2, dosarc2, arc2,c);
  //clear va supprimer le fichier qui va être écrasé.
  fin(fichier2);

  struct posix_header entete;
  ssize_t lect = read(fichier1,&entete,512);

  strncpy(entete.name,c,100);
  set_checksum(&entete);

  write(fichier2,&entete,512);

  char *tampon [512];
  char *ctaille = entete.size;
  int taille;
  int sc = sscanf(ctaille,"%o",&taille);
  int nb = ((taille+512-1)/512);

  for (int i = 0; i<nb;i++){
    read(fichier1,&tampon,512);
    write(fichier2,tampon,512);
  }
  char *tnull[1024];
  memset(tnull,(int)'\0',1024);
  write(fichier2,tnull,1024);
}

int cp_r_intertar(int fichier1, int fichier2, char *dosarc2, char *arc2, char *fic, char *c){
  //copie récursive dossier
  struct posix_header entete;
  if(rechercher(fichier1,0,fic)==1){
    int chm = strlen(fic);
    char *ctaille = entete.size;
    int taille;
    int sc = sscanf(ctaille,"%o",&taille);
    int nb = ((taille+512-1)/512);
    char tab[500];
    
    int v = copier(fic,tab,0);
    
    while(read(fichier1,&entete,512)>0){
       ctaille = entete.size;
       taille;
       sc = sscanf(ctaille,"%o",&taille);
       nb = ((taille+512-1)/512);
       if(strncmp(entete.name,fic,chm)==0){
	 copier(entete.name,tab,v);
	 cpfichier_intertar(fichier1, fichier2, dosarc2, arc2,entete.name, fic);
	 cleartab(v, tab);
       }
       else lseek(fichier1,nb*512,SEEK_CUR);
    }
    return 0;
  }
}

int cp_r_intratar(){
  return 0;
}
