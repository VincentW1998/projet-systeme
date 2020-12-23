#include "util_tar.h"
#include "tar.c"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/random.h>

int contient(char *dossier,char *nom){
  DIR *d = opendir(dossier);
  struct dirent *ds = readdir(d);
  while (1){
    if(readdir(d)<=0) break;
    if(strcmp(ds->d_name,nom)==0) return 1;
  }
  return 0;
}

int copier(char *a, char *b, int pos){
  //copie a dans b à la position pos
  int i=0;
  while(b[pos+i]=='\0' && a[i] != '\0' && pos+i < strlen(b) && i < strlen(a)){
    b[pos+i]=a[i];
    i++;
  }
  return pos+i;
}

/*int cleartab(int pos, char *b){
 * int i = 1;
 *while(pos+i<strlen(b)){
 *  b[pos+i]='\0';
 *  i++;
 *}
 *return 1;
}*/
  

int rmfichier_tar(int fichier,char *chemindossier,char *cheminarchive,char *c){
  if(rechercher(fichier,1,c)==1){
    off_t decalage = lseek(fichier,0,SEEK_CUR);
    
    struct posix_header entete;
    ssize_t lect = read(fichier,&entete,512);

    if(lect<=0){
      perror("test");
      return -1;
    }
    
    char *ctaille = entete.size;
    int taille;
    int sc = sscanf(ctaille,"%o",&taille);
    int nb = ((taille+512-1)/512)+1;

    if(entete.typeflag=='5'){
      return -2;
    }
    
    char r[250];
    strncpy(r,"temp1",5);
    char b[500];
    int v[1];
    int i = 5;
    while(contient(chemindossier,r)){
      getrandom(v,1,0);
      v[0]=(v[0]%10)+48;
      r[i]=v[0];
      i++;
    }
    int pos1 = copier(chemindossier,b,0);
    copier(r,b,pos1);
    
    int ft=open(b,O_CREAT|O_RDWR|S_IRWXU);

    if(ft<=0){
      perror("Ouverture f : ");
      return -1;
    }
    
    char tampon[512];
    debut(fichier);

    while(lseek(fichier,0,SEEK_CUR)<decalage){
      read(fichier,&tampon,512);
      write(ft,tampon,512);
    }
    
    if(lseek(fichier,0,SEEK_CUR)==decalage){
      lseek(fichier,nb*512,SEEK_CUR);
    }
      
    while(read(fichier,&tampon,512)){
      write(ft,tampon,512);
    }

    close(fichier);
    unlink(cheminarchive);
    rename(b,cheminarchive);
    //close ft;
    return ft;
    //return 1;
  }
  else return fichier;
}

int rm_r_tar(int fichier,char *chemindossier,char *cheminarchive,char *c){
   if(rechercher(fichier,1,c)==1){
     int chm = strlen(c);
     debut(fichier);
     struct posix_header entete;
     ssize_t lect = read(fichier,&entete,512);
     
     if(lect<=0){
       perror("test");
       return -1;
     }
    
     char r[250];
     strncpy(r,"temp1",5);
     char b[500];
     int var[1];
     int i = 5;
     while(contient(chemindossier,r)){
       getrandom(var,1,0);
       var[0]=(var[0]%10)+48;
       r[i]=var[0];
       i++;
     }

     int pos1 = copier(chemindossier,b,0);
     copier(r,b,pos1);
     int ft=open(b,O_CREAT|O_RDWR|S_IRWXU);
     
     char *ctaille;
     int taille;
     int sc;
     int nb;
     char *tampon[512];
     debut(fichier);
     while(read(fichier,&entete,512)>0){
       //printf("While ");
       
       if(strncmp(entete.name,c,chm)==0){
	 //printf("supprime\n");
	 ctaille=entete.size;
	 sscanf(ctaille,"%o",&taille);
	 nb = (taille+512-1)/512;
	 lseek(fichier,nb*512,SEEK_CUR);
       }

       else {
	 //printf("ajoute\n");
	 write(ft,&entete,512);
	 ctaille=entete.size;
	 sscanf(ctaille,"%o",&taille);
	 nb = (taille+512-1)/512;
	 for (int i =0 ; i < nb; i++){
	   read(fichier,&tampon,512);
	   write(ft,tampon,512);
	 }
       }
     }
     //printf("Je suis à la fin\n");

     close(fichier);
     unlink(cheminarchive);
     rename(b,cheminarchive);
     //close (ft)
     return (ft);
     //return 1;
   }
   else return fichier;
}

/**void main(){
  int f = open("t.tar",O_RDWR);
  
  if(f<=0){
    perror("Erreur ouverture");
    return;
  }

  //rm_r_tar(f,"./","./t.tar","a/");
  //rmfichier_tar(f,"./","./t.tar","a/c");
  close(f);
  }**/
