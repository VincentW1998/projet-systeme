#include "gestionnaire.h"
//#include "util_tar.h"
#include "tar.h"
#include "monrm.h"
#include "storeRestore.h"
#include "check.h"
#include "myCd.h"


int contient(char *dossier,char *nom){
  DIR *d = opendir(dossier);
  struct dirent *ds = readdir(d);
  while (1){
    if(readdir(d)<=0) break;
    if(strcmp(ds->d_name,nom)==0) return 1;
  }
  return 0;
}

/*int rmfichier_tar(int fichier,char *chemindossier,char *cheminarchive,char *c){
  if(rechercher(fichier,1,c)==1){
    off_t decalage = lseek(fichier,0,SEEK_CUR);
    
    struct posix_header entete;
    ssize_t lect = read(fichier,&entete,512);

    if(lect<0){
      perror("Erreur lecture");
      return fichier;
    }

    if(lect==0){
      //fin de la lecture
      return fichier;
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
    int var[1];
    int i = 5;
    
    while(contient(chemindossier,r) && i<250){
      //génère un nom aléatoire temporaire.
      getrandom(var,1,0);
      var[0]=(var[0]%10)+48;
      r[i]=var[0];
      i++;
    }
     
    int j = 0;
    while(chemindossier[j] != '\0'
    && j < 500  && j < strlen(chemindossier)){
      b[j]=chemindossier[j];
      j++;
    }
     
    int j2=0;
    while(j2+j < 500 && j2 < i){
      b[j2+j]=r[j2];
      j2++;
    }
     
    int ft=open(b,O_CREAT|O_RDWR,S_IRWXU);

    if(ft<0){
      perror("Création/Ouverture de ft dans rm");
      return fichier;
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
} */

int rmfichier_tar(char * path){
  int fd, n;
  storePosition(); //store sa position
  char * pathCd = subWithoutRepo(path); // path cd

  if(whichCd(pathCd) == -1)
    return -1;

  char *pathRm = subWithRepo(path); // file a supprimer

  if(TARPATH[0] == '\0') { // si t'es pas dans un tar alors tu appelles exec
    commandNoTar("rm", pathRm); // appel fonctions avec exec
    restorePosition(); // restorePosition
    return 1;
  }

  char * tarName = substringTar(); // recupere le nom du tar file
  fd = open(tarName, O_RDWR);
  if(fd < 0) {
    perror("open fichier tar");
    return -1;
  }

// getcwd + tarpath + fichier a supprimer
  char * pathWithFile = createPathFile(pathRm); 
  rmOn = 1;
  if((n = checkEntete(tarName, pathWithFile)) == -1) { //check si le fichier existe
    rmOn = 0;
    restorePosition();
    return -1;
  }
  rmOn = 0;
  restorePosition();
  return 1;
}


/*int rm_r_tar
(int fichier,char *chemindossier,char *cheminarchive,char *c){
  if(rechercher(fichier,1,c)==1){
    int chm = strlen(c);
    debut(fichier);
    struct posix_header entete;
    ssize_t lect = read(fichier,&entete,512);
     
    if(lect<0){
      perror("Erreur lecture");
      return fichier;
    }

    if(lect==0){
      return fichier;
    }
    
    char r[250];
    strncpy(r,"temp1",5);
    char b[500];
    int var[1];
    int i = 5;
    while(contient(chemindossier,r) && i<250){
      //génère un nom aléatoire temporaire.
      getrandom(var,1,0);
      var[0]=(var[0]%10)+48;
      r[i]=var[0];
      i++;
    }
     
    int j = 0;
    while(chemindossier[j] != '\0'
    && j < 500  && j < strlen(chemindossier)){
      b[j]=chemindossier[j];
      j++;
    }
     
    int j2=0;
    while(j2+j < 500 && j2 < i){
      b[j2+j]=r[j2];
      j2++;
    }
     
    int ft=open(b,O_CREAT|O_RDWR,S_IRWXU);
     
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
}*/

int rmTar(int nbOption, char ** command) {
  int i = 1;
  if(strcmp(command[1], "-r") == 0)
    i = 2;
  for(; i < nbOption; i++) {
    rmfichier_tar(command[i]);
//   rm_r_tar(f, cheminDossier, cheminArchive, c);
  }
  return 1;
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
