#include "gestionnaire.h"
#include "tar.h"

int affichagePrompt() { // affichage du prompt
  write(1, KBLU, strlen(KBLU));
  write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
  write(1, "> ", 2);
  write(1, RESET, strlen(RESET));
  return 0;
}

//affiche le message d'erreur si la commande est inconnue
int afficheMessageErreur(char ** command) {
  char * erreur = "tsh : command not found: ";
  char * erreurBuf = malloc(strlen(erreur) + strlen(command[0]) + 2);
  strcpy(erreurBuf, erreur);
  strcat(erreurBuf, command[0]);
  strcat(erreurBuf, "\n");
  write(2, erreurBuf, strlen(erreurBuf));
  return 0;
}

int execCommand(char ** command) {
  pid_t pid = fork();
  int n;
  if(pid != 0) { // on attend que le processus fils se lance
    wait(NULL);
  }
  else { // puis on execute la commande souhaité
    if((n = execvp(command[0], command)) == -1)
      afficheMessageErreur(command);
  }
  return 0;
}

void *lectureLigne(char * str, char * buff){ //readLine
  char * token = strtok(str,"\n");
  if(token!=NULL){
    buff = malloc(strlen(token) + 1);
    strcpy(buff,token);
    strcat(buff,"\n");
  }
  return buff;
}

int separateurCommand(char * buff, char ** command){ // separe la ligne en tableau de char
  char * token = strtok(buff, " \n");
  command[0] = malloc(strlen(token) + 1);
  strcpy(command[0], token);
  int i = 1;

  while((token = strtok(NULL, " \n")) != NULL) {
    command[i] = malloc(strlen(token) + 1);
    strcpy(command[i], token);
    i ++;
  }
  int nbOption = i;
  command[i] = NULL;
  command[i+1] = NULL;
  return nbOption;
}

int commandPersonnalisee(char ** command) {
  int nbCommand = 3;
  char * commandPerso[nbCommand];
  int numeroCommand = -1;
  commandPerso[0] = "exit";
  commandPerso[1] = "cd";
  commandPerso[2] = "pwd";

  for (size_t i = 0; i < nbCommand; i++) {
    if(!strcmp(commandPerso[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -2;
    case 0 : exit(0);
    case 1 :
      // chdir(command[1]);
      cdPerso(command[1]);
      break;
    default :
    write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
    write(1, "\n", 2);
    break;
  }
  return 0;
}

int commandTar(char ** command) {
  int nbCommand = 8;
  char *cmdTar[nbCommand];
  int numeroCommand = -1;
  cmdTar[0] = "pwd";
  cmdTar[1] = "cd";
  cmdTar[2] = "ls";
  cmdTar[3] = "mkdir";
  cmdTar[4] = "rmdir";
  cmdTar[5] = "mv";
  cmdTar[6] = "cat";
  cmdTar[7] = "cp";

  for (size_t i = 0; i < nbCommand; i++) {
    if(!strcmp(cmdTar[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -1;
    case 0 :
      printf("Here\n");
      write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
      write(1, "/", 1);
      write(1, TARPATH, strlen(TARPATH));
      write(1, "\n", 2);
      // break;
      return 0;
    case 1 :
      return navigateTar(command[1]);
  }
  return -1;

}

int estTar(char * token) {
  char * temp = malloc(strlen(token)+1);
  memcpy(temp,token,strlen(token));
  char * tok = strtok_r(temp, ".",&temp);
  char * name = NULL;

  while((tok = strtok_r(temp, ".\n",&temp)) != NULL) {
    name = malloc(strlen(tok) + 1);
    strcpy(name, tok);
  }
  if(name != NULL && !strcmp(name, "tar")){
    free(temp);
    return 0;
  }

  free(temp);
  return -1;
}

int existTar(char * token){
  DIR * dir = opendir(".");
  struct dirent * cur;
  while((cur = readdir(dir)) > 0){
    if(!strcmp(cur->d_name,token)) return 0;
  }
  closedir(dir);
  write(2,"no such file or directory:\n",strlen("no such file or directory:\n"));
  perror("error: ");
  return -1;
}

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

int hasTar(char * path){
  char * token;
  int i = 0;
  // for (size_t i = 1; i < nbOption; i++){
    char * tmp = malloc(strlen(path)+1);
    memcpy(tmp,path,strlen(path));

    while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
      if(!estTar(token)){
        free((tmp));
        return 0;
      }
      i++;
    }
    free(tmp);
  return -1;
}
void * findTar(char * path){
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));
  char * token;
  while((token = strtok_r(tmp, "/\n", &tmp)) != NULL)
    if(!estTar(token)) return token;
  return NULL;
}

int navigateTar(char * path){
  char * tarFile = findTar(path);
  char * token;
  int file,n;
  printf("1\n");
  char * tmp = malloc(strlen(TARPATH)+1);
  memcpy(tmp,TARPATH,strlen(TARPATH));
   // ne prend pas en compte les cas imbriques donc si il y a  un tar dans le path
   // il s'agit d'un path absolu
  if(tarFile != NULL){
     // if(absolutePath(path)) return 0;
     return 0;
  }//relative path
  // printf("2\n");

  token = strtok_r(tmp,"/\n",&tmp);
  char * fullpath = malloc(sizeof(char)+1); // concatenation de TARPATH(sans le tar) et la destination
  // memcpy(fullpath, tmp, strlen(fullpath));
  if(tmp!=NULL)
    memcpy(fullpath, tmp, strlen(tmp));

  // printf("3\n");
  //
  strcat(fullpath, path);
  // printf("%s\n", fullpath);
  // printf("4\n");

  //on entre dans le tarball
  if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}
  // printf("5\n");
  // if(fullpath[strlen(fullpath-1)])
  struct posix_header * p = malloc(sizeof(struct posix_header));
  while((n = read(file,p,BLOCKSIZE))>0){
    // printf("6\n");
    // printf("%s\n", fullpath);

    if(!strcmp(p->name, fullpath)){
      // printf("7\n");
      // memcpy(TARPATH,fullpath,strlen(fullpath));
      // printf("%s\n",TARPATH );
      if(TARPATH[strlen(TARPATH-1)]!='/') strcat(TARPATH, "/");
      
      strcat(TARPATH,path);
      // printf("endRes :%s\n", TARPATH);
      return 0;
    }
    lseek(file,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
  }
  // printf("8\n");

  free(fullpath);
  free(tmp);
  return -1;
}

// int hasDotDot(char * path){
//   char * tmp = malloc(strlen(path)+1);
//   strcpy(tmp,path);
//
// }


int cdPerso(char * path){
  if(!hasTar(path)){
    if(TARPATH==NULL){
      // printf("%s!\n",command[nbOption-1] );
       char * tmp = cd(path);

      // char * tmp = cd(nbOption, command);
      if(tmp != NULL) {
        TARPATH = malloc(strlen(tmp) + 1);
        strcpy(TARPATH, tmp);
        return 0;
      }
      return -1;
    }
    else{
      // navigateTar(nbOption,command);
      return 0;
    }
  }
  chdir(path);
  return 0;
}


void * cd (char * path) {
  char * basicPath = malloc(sizeof(char)+1);
  // char * token2;
  char * token;
  // for (size_t i = 1; i < nbOption; i++){
    char * tmp = malloc(strlen(path)+1);
    memcpy(tmp,path,strlen(path));
    while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
      if(!estTar(token)){
        // printf("%lu\n",strlen(basicPath) );
        if(strlen(basicPath) == 0){
          if(!existTar(token))
            return token;
          }
        else {
          if(!moveTo(basicPath,token)){
            return token;
          }
        }
      }
      strcat(basicPath,token);
      strcat(basicPath,"/");
    }
    free(tmp);
    free(basicPath);
  return NULL;
}

// void * findTar(int nbOption, char ** command) {
//   char * basicPath;
//   // char * token2;
//   char * token;
//   printf("%s\n",command[1] );
//   for (size_t i = 1; i < nbOption; i++){
//     // char * tmp = malloc(strlen(command[i]) + 1);
//     // strcpy(tmp, command[i]);
//     // token = strtok_r(tmp, "/\n",&tmp);
//     // // char * tmp2 = malloc(strlen(token) + 1);
//     // // strcpy(tmp2, token);
//     // if(!estTar(token) && !existTar(token)){
//     //   printf("here\n");
//     //   return token;
//     // }
//     char * tmp = malloc(strlen(command[i])+1);
//     memcpy(tmp,command[i],strlen(command[i]));
//     token = strtok_r(tmp,"/\n",&tmp);
//     if(!estTar(token) && !existTar(token)){
//       printf("here\n");
//       return command[i];
//     }
//
//     basicPath = malloc(strlen(token)+1);
//     strcpy(basicPath, token);
//
//     strcat(basicPath,"/");
//
//     while((token = strtok_r(tmp, "/\n",&tmp)) != NULL) {
//       printf("%s\n",token);
//       // char * tmp = malloc(strlen(token2) + 1);
//       // printf("2\n");
//       if(!estTar(token)){
//         // printf("token2 : %s\n", token2);
//           if(!moveTo(basicPath,token)){// bouge sur token 2
//             printf("%s\n",command[1] );
//             return command[1];
//           }
//           printf("1\n");
//       }
//       strcat(basicPath,token);
//       strcat(basicPath,"/");
//     }
//     free(tmp);
//     free(basicPath);
//   }
//
//   return NULL;
// }
