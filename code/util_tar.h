#ifndef UTILTAR_H
#define UTILTAR_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
//#include "parcours.h"

/* Programme modifiant les entêtes contenus
 * dans les «tar». Il s'applique en fonction
 * du «décalage» courant.
 */

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
  
  if(lect==0)
    return -1;
  
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
#endif
