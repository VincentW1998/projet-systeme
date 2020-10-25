#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tar.h" // Programme fourni dans le TP1 du cours de Système

//Suppression des fichiers, uniquement dans les tarballs pour le moment.
int compression(int fichier);


int suppression(int fichier,int nb){
  //suppression du fichier
  char tab [512];
  for(int i = 0 ; i < 512;i++){//Remplit un tableau de caractères nuls
    tab[i]='\0';//Caractère nul
  }
  for (int i = 0; i < nb+1; i ++){
    write(fichier,tab,512);//On supprime par bloc de 512.
  }
}

int base_rm (int fichier, char *adresse, char *actuel){
  struct posix_header entete;
  ssize_t lect = read (fichier, &entete, 512);
  
  if(lect <= 0){
    perror("Echec de la lecture.");
    return -1;
  }
  char *c = entete.size;
  int b;
  int sc = sscanf(c,"%o",&b);
  int nb = (b+512-1)/512;
  printf("Valeur de b : %d\n", b);
  if(strcmp(entete.name,adresse)==0){
    // On repositionne la curseur avant l'entete qu'on vient de lire pour la supprimer
    lseek(fichier,-512,SEEK_CUR);
    return suppression(fichier,nb);
  }
  printf("Test !");
  if(nb==0) return -1;
  lseek(fichier,nb*512,SEEK_CUR);
  return base_rm(fichier, adresse, actuel);
}

int rm_r(int fichier, char *adresse, char *actuel);

int main(int argc, char **argv){
  int fichier = open (argv[1],O_RDWR);
  if(argc = 2);
  if(fichier <0){
    perror("Erreur à l'ouverture du fichier.");
    return 1;
  }
  return base_rm(fichier, argv[2],"");
}

