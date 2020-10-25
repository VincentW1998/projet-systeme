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
#include "tar.h"

int deplacer(int fichier, struct posix_header entete,char *destination){
  //déplacement brutal, sans vérification d'existance du chemin
  strcpy(entete.name,destination);
  write(fichier,&entete,512);
  return 0;
}

int mv(int fichier, char *source, char *cible){
  struct posix_header entete;
  ssize_t lect = read (fichier, &entete, 512);

  if(lect <= 0){//lecture impossible
    perror("Échec de la lecture.");
    return -1;
  }

  char *c = entete.size;
  int b;
  // int sc = sscanf(c,"%o",&b);
  sscanf(c,"%o",&b);

  int nb = (b+512-1)/512;

  if(strcmp(entete.name,source)==0){
    // Vérifie que le chemin existe bien. Si oui, on tente de copier.
    lseek(fichier,-512,SEEK_CUR);
    return deplacer(fichier,entete,cible);
  }

  if(nb==0) return -1;
  lseek(fichier,nb*512,SEEK_CUR);
  return mv(fichier,source,cible);
}

/** Pour effectuer des tests **/
int main (int argc, char **argv){
  if(argc < 4) {
    perror("usage : ./%s fichier.tar fichierSource fichierTarget");
    exit(1);
  }
  int fichier = open(argv[1],O_RDWR);
  //Lecture et écriture du fichier.
  if(fichier < 0) {
    perror("Erreur à l'ouverture du fichier.");
    return 1;
  }

  mv(fichier, argv[2], argv[3]);
  return 0;
}
