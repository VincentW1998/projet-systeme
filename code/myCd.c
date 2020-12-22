#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"
//#include "tar.h"

int cdNoOptions(){
  if(*TARPATH != '\0') setTarpath("\0");
  chdir(getenv("HOME"));
  return 0;
}

// fonction qui appelle cdPerso = commandPersonnalisee
int cdPerso(char * path){
  memset(TARPATH,0, strlen(TARPATH));
  if(path[0] == '/')return cdAbs(path);
  if(hasTar(path) == 0) return cd(path); // si dans le path il y un tar
  chdir(path);
  return 0;
}

int cd(char * path ){
  //if(path[0] == '/')return cdAbs(path);
  char * tmp = malloc(strlen(path) + 1), *stdPath, *tbPath, *token;
  strcpy(tmp, path);
  int l = 0;
  while((token = strtok_r(tmp, "/", &tmp)) != NULL){
    if(estTar(token) == 0) break;
    l += 1 + strlen(token);
  }
  if(l > 0){ // si il y un path avant le tar
    if(path[0] != '/') l--;
    stdPath = malloc(l + 1);
    strncpy(stdPath, path, l);
    chdir(stdPath);
    free(stdPath);
  }
  if(l < strlen(path)){
    if(existTar(token) == -1){
      perror("le fichier n'existe pas");
      return -1;
    }
    setTarpath(token);
    if( (l + strlen(token)) == strlen(path) ) return 0;
    l += strlen(token) + 1; // on rajoute la taille du tar au nb de char a sauter + /
    tbPath = malloc(l + 1);
    strcpy(tbPath, path+l);
    if((tbPath[0] == '/' && navigate(tbPath+1) == -1 )|| (tbPath[0] != '/' && navigate(tbPath) == -1)){
      setTarpath("\0");
      free(tbPath);
      return -1;
    }
    free(tbPath);
  }
  else setTarpath("\0");
  return 0;
}

// se charge du path depuis un tarball
// NE SE CHARGE PAS DE RENTRER DANS LE .TAR (voir cd pour ça)
int navigate(char * path){
  //	if(path[0] == '/') return cdPerso(path); // si le path est absolu
  if(path[0] == '/') return cdAbs(path);
  char *fullpath[50], *tmp, *token, *tar;
  tmp = malloc(strlen(TARPATH) + 1);
  strcpy(tmp, TARPATH);
  tar = strtok_r(tmp, "/\0",&tmp); // on stock le .tar
                   //	tar = resTar(tar); // resolution pb char etrange apres le .tar
  
  int i = 0; // nb d'elements dans TARPATH;
  int l = 0; //nb de char parcourus dans path
  if(strlen(tar) < strlen(TARPATH)){ //on stock le tarpath dans FP
    for(;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++){
      fullpath[i] = malloc(strlen(token) + 1);
      strcpy(fullpath[i], token);
    }
  }
  // on passe au path
  tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path); // on copie le path
  for(;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++, l+= strlen(token) + 1){
    
    if(strcmp(token, "..") == 0){
      if(i == 0){
        l+= strlen(token);
        if(l == strlen(path)){ // si on atteint la fin du path
          setTarpath("\0");
          return 0;
        }
  //      return cdPerso(path + strlen(tmp));
          return cdPerso(tmp);
      }
      if(checkfp(tar, fullpath, i) == -1){perror("cd");return -1;} // exit
      free(fullpath[i-1]);
      i -= 2;
    }
    else{
      if(estTar(token) != 0){
  //strcpy(TARPATH, posTar);
  //strcpy(TARPATH, posTar);
        fullpath[i] = malloc(strlen(token) + 1);
        strcpy(fullpath[i], token);
      }
      else i--;
      
    }
    
  }
  if(i == 0){
    setTarpath(tar);
    return 0;
  }
  
  if(checkfp(tar, fullpath, i) == -1){ perror("cd :"); return -1; }//exit
  
  for(int x = 1; x < i; x++){
    fullpath[0] = realloc(fullpath[0], strlen(fullpath[0]) + strlen(fullpath[x]) + 2);
    strcat(fullpath[0], "/"); // warning
    strncat(fullpath[0], fullpath[x], strlen(fullpath[x]));
    free(fullpath[x]); fullpath[x] = NULL;
  }
  tmp = malloc(strlen(tar) + strlen(fullpath[0]) + 2);
  strcpy(tmp, tar);
  strcat(tmp, "/"); //warning
  strncat(tmp, fullpath[0], strlen(fullpath[0]));
  
  setTarpath(tmp);
  free(fullpath[0]);
  free(tmp); tmp = NULL;
  return 0;
  
}


// s'occupe des path commançant par / (path absolu)
// fait appel a cd dès qu'il trouve un .tar
int cdAbs(char * path){ // temporary fix
  storePosition();
  char  * tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);
  char * token;
  for(int n = 0;(token = strtok_r(tmp, "/\n", &tmp)) != NULL; n += strlen(token) +1){
    if(estTar(token)==0){
      if(n == 0)cdNoOptions();
      else{
        tmp = NULL;
        tmp = malloc( n + 1);
        strncpy(tmp, path, n);
        if(chdir(tmp) == -1){
          return -1;
        }
      }
      if(n < strlen(path) && cd(path+n+1) == -1){
        restorePosition();
        return -1;
      }
      return 0;
    }
  }
  return chdir(path);
}


// Check le FullPath fonction utilisee dans NAVIGATE
int checkfp(char *tar, char *fullpath[50], int i){
  char * path = malloc(strlen(fullpath[0]) + 2);
  strcpy(path, fullpath[0]);

  for(int x = 1; x < i; x++){
    path = realloc(path, strlen(path) + strlen(fullpath[x]) + 2);
    strcat(path, "/");
    strncat(path, fullpath[x], strlen(fullpath[x]));
  }

  strcat(path, "/");
//	if(checkPath(path, tar) == -1) {free(path); return -1;}
  if (checkEntete(tar, path) == -1) {free(path); return -1;}

  free(path);
  return 0;
}

void setTarpath(char * tarp){
  TARPATH = NULL; //  au moins sur de plus rien avoir dedans
  TARPATH = malloc(strlen(tarp) + 1);
  strcpy(TARPATH,tarp);
}

int whichCd(char * pathCd) {
  //if tarpath vide -> cdPerso because we are not in tar file
  if (TARPATH[0] == '\0') {
    if(cdPerso(pathCd) == -1) return -1;
  }
  else
    if(navigate(pathCd) == -1) return -1;
  return 1;
}
