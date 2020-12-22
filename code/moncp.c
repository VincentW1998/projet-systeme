#include "util_tar.h"
#include "tar.c"
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

int cpfichier_intertar(int fichier1, int fichier2, char *fic, char *c){
  rechercher(fichier1,1,fic);
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
