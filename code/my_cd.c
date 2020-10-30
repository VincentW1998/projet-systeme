#include "my_cd.h"
#include "gestionnaire.h"
#include "tar.h"
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
  if(TARPATH != NULL) // docker
  //if(TARPATH[0]=='\0')
    TARPATH = NULL;
  chdir(getenv("HOME"));
  return 0;
}

// fonction pere = commandTar
int navigate(char * path){// ..
  char * fullpath[100]; // liste qui va separer le path
  char * token;
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));

  int i = 0;
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    if(!strcmp(token,"..")){
      if(i == 0) dotdot(tmp); //si le path nous mene hors du tarball
      else{
        free(fullpath[i-1]); // on recule d'un cran dans le path
        i--;
      }
    }
    else { // si c'est pas ..
      fullpath[i] = malloc(strlen(token)+1);
      memcpy(fullpath[i],token,strlen(token));
      strcat(fullpath[i],"/");
      i++;
    }

  }
  if(i == 0)return 0;//si le fullpath est null on sort

  //transformation du tableau de string("java") en chaine de caractere
  for(int x = 1;x<i;x++){
    strcat(fullpath[0],fullpath[x]);
    // printf("fullpath[0] : %s\n", fullpath[0]);
  }

  char * tarp = malloc(strlen(TARPATH)+1);
  //memcpy(tarp,TARPATH,strlen(TARPATH));// tarp = TARPATH copie
  strcpy(tarp, TARPATH);
  token = strtok_r(tarp,"/", &tarp);
//docker
  if(tarp != NULL){
    //if(tarp[0]!='\0'){
    char * tmp2 = malloc(strlen(tarp) + strlen(fullpath[0]) + 2);
    strcpy(tmp2,tarp);
    strcat(tmp2, "/");
    strncat(tmp2,fullpath[0],strlen(fullpath[0])); // vide le token ici jsp pourquoi

    return checkPath(tmp2, token); // token toujours le fichier.tar
  }
  return checkPath(fullpath[0], token);

}

int checkPath(char * path, char * token){
  printf("le fichier tar : %s!\n", token);
  int file, n;
  if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}

  struct posix_header * p = malloc(sizeof(struct posix_header));

  while((n = read(file,p,BLOCKSIZE))>0){
    printf("%s!\n",p->name );
    if(((p-> typeflag == '5') && (!strncmp(path, p -> name, strlen(path)))) ){

      printf("checkpath while\n");
      // if(TARPATH[strlen(TARPATH-1)]!='/') strcat(TARPATH, "/");
      if(token[strlen(token) -1] != '/'){
        strcat(token, "/");
      }
      TARPATH = NULL;
      if(path[strlen(path)-1]=='/') {
        // strncat(TARPATH,path,strlen(path)-1);

        TARPATH = malloc(strlen(token) + strlen(path));
        strcpy(TARPATH, token);
        strncat(TARPATH, path, strlen(path) - 1);
      }

      else {
        // strcat(TARPATH,path);
        TARPATH = malloc(strlen(token) + strlen(path) + 1);
        strcpy(TARPATH, token);
        strncat(TARPATH, path, strlen(path));
      }
      printf("TARPATH at the end : %s!\n", TARPATH);
      close(file);
      return 0;
    }
    lseek(file,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
  }
  close(file);
  return -1;
}

// fonction pere : navigate
int dotdot(char * path){//..
  char * token;
  char * tmp2 = malloc(sizeof(char)+1);
  if(TARPATH == NULL){   //tarpath null

    if(findTar(path) == NULL){ // il y a pas de tar
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

  // ici TARPATH != NULL
  char * tmp = malloc(strlen(TARPATH)+1);
  memcpy(tmp,TARPATH,strlen(TARPATH));
  // printf("Avant le while\n");
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    // printf("Dans le while\n");
    // printf("tmp : %s\n", tmp);
    if(tmp == NULL){// Pas SUR
    //if(tmp[0]=='\0'){//docker

      if(strlen(tmp2) != 0){
        // printf("tmp2 :%s\n", tmp2);
        TARPATH = NULL;
        TARPATH = malloc(strlen(tmp2));
        strncpy(TARPATH,tmp2, strlen(tmp2) - 1);
        // printf("tarpath apres copy tmp2 : %s\n", TARPATH);
      }
      else TARPATH = NULL;

      return 0;
    }
    strcat(tmp2,token);
    strcat(tmp2,"/");
  }


  return -1;
}

// fonction qui appelle cdPerso = commandPersonnalisee
int cdPerso(int nbOption, char * path){
  if(!hasTar(path)){ // si dans le path il y un tar
    if(TARPATH==NULL){
        // printf("cdPerso path :%s\n", path);
      //  char * tmp = cd(path);
      // if(tmp != NULL) {// en gros si path est du genre fichier.tar
      //   TARPATH = malloc(strlen(tmp) + 1);
      //   strcpy(TARPATH, tmp);
      //   return 0;
      // }
      // return -1;
      return cd(path);
    }
    else{
      return 0;
    }
  }
  chdir(path);
  return 0;
}
int cd(char * path){
  char * basicPath = malloc(sizeof(char)+1);
  // char * tmp = malloc(strlen(path)+1);
  char * token;
  // strncpy(tmp,path,strlen(path));
  while((token = strtok_r(path,"/\n",&path))!=NULL){
    if(!estTar(token)){
      if(basicPath[0] != '\0' && moveTo(basicPath, token)) return -1;
      if(existTar(token)) return -1;
      TARPATH = malloc(strlen(token)+1);
      strncpy(TARPATH,token,strlen(token));
      if(path == NULL) return 0;
      break;
    }
    strncat(basicPath, token,strlen(token));
    strncat(basicPath,"/",1);
  }
  if(navigate(path)){
    TARPATH = NULL;
    return -1;
  }
  return 0;

  // char * tmp2 = malloc(strlen(tmp)+1);
  // strncpy(tmp2,tmp,strlen(tmp));
  // if(checkPath(tmp2, token)) return -1;

  // printf("TARPATH:%s!\n",TARPATH );
  // printf("tmp:%s!\n", tmp);
  // TARPATH = realloc(TARPATH, strlen(token) + strlen(tmp) + 1);
  // strncat(TARPATH, tmp, strlen(tmp));
  // return 0;
}

// void * cd (char * path) { //ex: path = leTest.tar
//   char * basicPath = malloc(sizeof(char)+1);
//   char * token;
//   char * tmp = malloc(strlen(path)+1);
//   memcpy(tmp,path,strlen(path));
//   while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
//     if(!estTar(token)){
//       if(strlen(basicPath) == 0){
//         if(!existTar(token))
//           return token;
//       }
//       else {
//         if(!moveTo(basicPath,token)){
//           return token;
//         }
//       }
//     }
//     strcat(basicPath,token);
//     strcat(basicPath,"/");
//   }
//   // free(tmp);
//   // free(basicPath);
//   return NULL;
// }
