#ifndef PARCOURS_H
#define PARCOURS_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "tar.h"

int debut(int fichier){
  return lseek(fichier,0,SEEK_SET);
}

int suivant(int fichier){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);

  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }

  if(entete.name[0]=='\0'){
    lseek(fichier,-512,SEEK_CUR);
    return 2;
  }
  char *ctaille = entete.size;
  int taille;
  int sc = sscanf(ctaille,"%o",&taille);
  int nb = (taille+512-1)/512;
  
  lseek(fichier,nb*512,SEEK_CUR);
  return 0;
}

static int recherche(int fichier, int option, char *nom){
  struct posix_header entete;
  ssize_t lect = read(fichier,&entete,512);

  if(lect==0) return -1;
  
  if(lect<0){
    perror("Echec de la lecture");
    return -1;
  }

  if(strcmp(entete.name,nom)==0){
    if(option == 1) lseek(fichier,-512,SEEK_CUR);
    else debut(fichier);
    return 1;
  }

  lseek(fichier,-512,SEEK_CUR);
  if (suivant(fichier)==0)
    return recherche(fichier,option,nom);
  else return -1;
}

int rechercher (int fichier, int option, char *nom){
  debut(fichier);
  recherche(fichier, option, nom);
}

int fin(int fichier){
  debut(fichier);
  while(1){
    int x = suivant(fichier);
    if(x==2) break;
    else if(x==-1) return -1;
  }
  return 0;
}
#endif
