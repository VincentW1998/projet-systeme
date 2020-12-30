#include "gestionnaire.h"
#include "myCd.h"
#include "myCat.h"
#include "check.h"
#include "storeRestore.h"

//imprime le contenu d'un fichier sur la sortie standard
int showContent(int file, int filesize){
  char buffer [BLOCKSIZE];
  ssize_t n;
  int nbBlock = (filesize + BLOCKSIZE - 1) / BLOCKSIZE;
  for(int i=0; i < nbBlock; i++) // on lis le contenu bloc par bloc
    if((n = read(file, buffer, BLOCKSIZE)) > 0)
      write(1,buffer,n);
  close(file);
  write(1, "\n", 1);
  return 1;
}

//cherche le fichier a lire dans le tarball et renvoie le posixheader correspondant
int readFile(char * path, char * tar){
  int f;
  size_t n;
  int filesize;
  if((f = open(tar, O_RDONLY)) == -1)
    return -1;
  struct posix_header * p = malloc(sizeof(struct posix_header));
  while( ((n = read(f,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0') ){
    sscanf(p -> size, "%o", &filesize);
    if(strcmp(p->name, path) == 0) return showContent(f,filesize); // check si le nom du fichier correspond bien au path
    next_header(f, filesize); // saute les blocs de contenu de chaque entete
  }
  close(f);
  return displayError("cat : file do not exist");
}

//effectue cat sur le path donné
int CatFile (char * path){
  char * lastToken = getLastToken(path);
  char * deplacement = pathWithoutLastToken(path, lastToken);
  if(whichCd(deplacement) == -1){
    free(deplacement);
    return -1;
  }
  free(deplacement);
  char * pathCat = createPathFile(lastToken);
  if(TARPATH[0] == '\0') return commandNoTar("cat", lastToken); //si hors du tarball exec
  char * tar = findTar(TARPATH);
  return readFile(pathCat, tar); //readfile sur le path apres le TAR
}

/* fonction principale cat effectue une boucle pour appeler cat sur tout les path donné
 C'est la fonction qui va etre appelée dans gestionnaire */
int cat(int nbOption, char ** path){
  if(nbOption < 2) return execCommand(path); // si la path n'a qu'un seul element cad cat return exec
  storePosition();
  for(int i=1 ; i<nbOption;i++){ // boucle qui va appeler cat sur chaque path
    if(CatFile(path[i]) == -1){
      restorePosition();
      return -1;
    }
    restorePosition();
  }
  return 1;
}

