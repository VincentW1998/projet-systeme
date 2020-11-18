#include "my_cd.h"
#include "gestionnaire.h"
// #include "tar.h"
int moveTo(char * path, char * tarball){
  char * pwd = getcwd(NULL, 0);

  if(chdir(path)){
    perror("pathError:");
    return -1;
  }
  if(!existTar(tarball))
    return 0;
  chdir(pwd);
  return -1;
}

int cdNoOptions(){
  if(*TARPATH != '\0') // docker
    strcpy(TARPATH,"\0");
  //if(TARPATH[0]=='\0')
    // TARPATH = NULL;
  chdir(getenv("HOME"));
  return 0;
}

// fonction pere = commandTar
int navigate(char * path){ // ..
  if(path[0] == '/') return cd(path); // si le path est absolu
  char * fullpath[100]; // liste qui va separer le path
  char * token;
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));

  int i = 0;
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    printf("dans le while\n");
    if(!strcmp(token,"..")){

      if(i == 0) dotdot(tmp); //si le path nous mene hors du tarball
      else{
        fullpath[i-1] = NULL;// on recule d'un cran dans le path
        i--;
      }
    }
    else { // si c'est pas ..
      fullpath[i] = malloc(strlen(token)+1);
      memcpy(fullpath[i],token,strlen(token));
      i++;
    }
  }
  if(i == 0)return 0;//si le fullpath est null on sort

  //transformation du tableau de string("java") en chaine de caractere
  for(int x = 1;x<i;x++){
    strcat(fullpath[0],"/");
    strcat(fullpath[0],fullpath[x]);
  }
  if(*TARPATH == '\0') cdPerso(fullpath[0]);

  printf("FULLPATH:%s!\n",fullpath[0]);
  char * tarp = malloc(strlen(TARPATH)+1); //problem
  //memcpy(tarp,TARPATH,strlen(TARPATH));// tarp = TARPATH copie
  strcpy(tarp, TARPATH);

  token = strtok_r(tarp,"/",&tarp);
  // char * tarfile = malloc(strlen(token)+1);
  // strcpy(tarfile, token);
  // token = strtok_r(tarp,"/",&tarp);
  // rest = strtok_r(tarp,"/",&tarp);
//docker
  if((strlen(TARPATH) - strlen(token)) > 0){ // pb docker
    // if(tarp[0]!='\0'){
    printf("tarp pas null\n");
    char * tmp2 = malloc(strlen(tarp) + strlen(fullpath[0]) + 2);
    strcpy(tmp2,tarp);
    strcat(tmp2, "/");
    strncat(tmp2,fullpath[0],strlen(fullpath[0]));
    return checkPath(tmp2, token, 5); // token toujours le fichier.tar
  }
  return checkPath(fullpath[0], token, 5);

}

// fonction pere : navigate
int dotdot(char * path){//..
  char * token;
  char * tmp2 = malloc(sizeof(char)+1);
  if(*TARPATH == '\0'){   //tarpath null
    // printf("yes\n");
    // if(*path == '\0') docker
    if(path == NULL) return chdir("..");
    if(findTar(path) == NULL){ // il n'y a pas de tar
      printf("pas de tar\n");
      return chdir(path);
    }
    else { //il y un tar
      while((token = strtok_r(path,"/\n",&path))!=NULL){
        if(!estTar(token)) break;
        strcat(tmp2,token);
        strcat(tmp2,"/");
      }
      if(chdir(tmp2)) return -1;
      memcpy(TARPATH,token,strlen(token));
      return 0;
    }
  } // fin TARPATH == NULL
printf("dotdot\n");
  // ici TARPATH != NULL
  char * tmp = malloc(strlen(TARPATH)+1);
  memcpy(tmp,TARPATH,strlen(TARPATH));
  // int length = 0;
  while((token = strtok_r(tmp,"/",&tmp))!=NULL){
    tmp2 = realloc(tmp2,strlen(token)+1);
    strcpy(tmp2,token);
  }

  if((strlen(TARPATH) - strlen(tmp2))!=0){
    char * newTARPATH = malloc(strlen(TARPATH) - strlen(tmp2));
    strncpy(newTARPATH, TARPATH,strlen(TARPATH) - strlen(tmp2) - 1);
    TARPATH = realloc(TARPATH, strlen(newTARPATH) + 1);
    strcpy(TARPATH, newTARPATH);
  }
  else {
    TARPATH = realloc(TARPATH, 1);
    strcpy(TARPATH, "\0");
  }
  return 0;
}

// fonction qui appelle cdPerso = commandPersonnalisee
int cdPerso(char * path){
  if(!hasTar(path)) return cd(path); // si dans le path il y un tar
  printf("lepath:%s\n", path);
  chdir(path);
  return 0;
}

int cd(char * path){
  printf("cd\n");
  char * basicPath = malloc(sizeof(char)+1);
  char * token;
  if(path[0]=='/') strcpy(basicPath,"/");
  while((token = strtok_r(path,"/\n",&path))!=NULL){
    if(!estTar(token)){
      if(basicPath[0] != '\0' && moveTo(basicPath, token)) return -1;
      if(existTar(token)) return -1;
      TARPATH = realloc(TARPATH, strlen(token)+1);
      strncpy(TARPATH,token,strlen(token));
      printf("peut etre\n");
      if(path == NULL) return 0; //pb
      printf("non\n");
      break;
    }
    strncat(basicPath, token,strlen(token));
    strncat(basicPath,"/",1);
  }
  printf("le reste du path:%s!\n",path );
  if(navigate(path)){
    strcpy(TARPATH,"\0");
    return -1;
  }
  return 0;
}
