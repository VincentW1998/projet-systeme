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

/** 
    Fonctionne uniquement dans les archives tar pour le moment.
    Ce programme copie correctement les fichiers (dans les tarballs) et les données associées.
    Cependant, il les écrit trop loin à la fin du fichier (à cause O_APPEND), 
    ce qui fait que tar -xvf ne detecte pas le fichier quand l'on désarchive. 
**/

int ecrire (char *destination, int nb_blocs, struct posix_header entete,int fichier){
  off_t sk = lseek(fichier,SEEK_CUR,SEEK_CUR)-1;
  strcpy(entete.name,destination);
  set_checksum(&entete);
  write(fichier,&entete,512);
  char tampon [512];
  for(int i = 0; i < nb_blocs ; i++){
    lseek(fichier,sk,(i*512));
    ssize_t taille = read(fichier,tampon,512); 
    write(fichier,tampon,512);
  }
  free(destination);
  return 0;
}

/**
   Prend en entrée un identifiant de fichier, une source pour le fichier que l'on 
   souhaite copier et une destination, et enfin la position actuelle. 
**/

int base_cp (int fichier, char *source, char *destination,  char *actuel){
  char *chemin = malloc (sizeof(actuel)+sizeof(destination));
  strcat(chemin,actuel);
  strcat(chemin,destination);
  // Ne prend pas encore en compte les chemins avec ".." et "."
  // Eventuellement faire une fonction qui les transformeraient en un chemin "propre".
  
  struct posix_header entete;
  ssize_t lect = read (fichier, &entete , 512);
  if(lect <= 0){//lecture impossible
    free(chemin);
    perror("Échec de la lecture.");
    return -1;
  }
  
  char *c = entete.size;
  int b;
  int sc = sscanf(c,"%o",&b);
  int nb = (b+512-1)/512;
  
  if(strcmp(entete.name,source)==0){
    // Vérifie que le chemin existe bien. Si oui, on tente de copier.
    return ecrire(chemin,nb,entete,fichier);
  }
  
  lseek(fichier,SEEK_CUR,nb*512);
  return base_cp(fichier, actuel,source, destination);
}

/** Pour effectuer des tests **/
int main (int argc, char **argv){
  int fichier = open(argv[1],O_RDWR | O_APPEND);
  //Lecture et écriture du fichier.
  if(fichier < 0) {
    perror("Erreur à l'ouverture du fichier.");
    return 1;
  }
  base_cp(fichier, argv[2], argv[3], "");
  return 0;
}
