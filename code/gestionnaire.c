#include "gestionnaire.h"
#include "tar.h"

int affichagePrompt() { // affichage du prompt
  write(1, KBLU, strlen(KBLU));
  write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
  if(TARPATH!=NULL){
    write(1,"/", 1);
    write(1, TARPATH, strlen(TARPATH));
  }
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

//Utilisation de execvp pour les commandes externes du shell
int execCommand(char ** command) {
  pid_t pid = fork();
  int n, w;
  switch (pid) {
    case -1:
      perror("fork");
      break;

    case 0:
      if((n = execvp(command[0], command)) == -1)
          afficheMessageErreur(command);
      break;
    default :
      wait(&w);
  }
  return 0;
}

// pour les commandes externes du shell avec un pipi
int execCommandPipe(char ** command, char ** commandPipe) {
  int fd[2];
  if(pipe(fd) < -1){
    perror("pipe");
  }
  int n, w;
  pid_t pid1, pid2;

  pid1 = fork();

  switch (pid1) {
    case -1 :
      perror("fork");
      break;

    case 0 : // fils, commandPipe[0], lecteur, fd[0]
      close(fd[1]);
      dup2(fd[0], 0);
      close(fd[0]);
      if((n = execvp(commandPipe[0], commandPipe)) == -1)
        exit(1);
        // perror("execvp");
        // afficheMessageErreur(commandPipe);
      break;
    default :
      pid2 = fork();

      switch (pid2) {
        case -1 :
          perror("fork");
          break;

        case 0 : // fils, command, ecrivain, fd[1]
          close(fd[0]);
          dup2(fd[1], 1);
          close(fd[1]);
          if((n = execvp(command[0], command)) == -1)
            exit(1);
            // perror("execvp");
            // afficheMessageErreur(command);
          break;
      }
      wait(&w); //attend le processus fils
  }
  return 0;
}

//fonction qui se comporte comme readLine
void *lectureLigne(char * str, char * buff){
  char * token = strtok(str,"\n");
  if(token!=NULL){
    buff = malloc(strlen(token) + 1);
    strcpy(buff,token);
    strcat(buff,"\n");
  }
  return buff;
}

// separe la ligne en tableau de char
int separateurCommand(char * buff, char ** command){
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

// si la ligne de commande contient un pipe
// on remplit command[] avec  tout les char avant le pipe |
// on remplit commandPipe[] avec tout les char apres le pipe |
// et on execute execCommandPipe
// si la ligne ne contient pas de pipe alors on execute tout simplement
// execcommand
void findPipeAndExec(int nbOption, char ** command, char ** commandPipe) {
  int pipe = 0;
  int i, j = 0;

  for(i = 0; i < nbOption; i ++) {
    if(!strcmp(command[i], "|")){
      pipe = 1;
      break;
    }
  }
  if (pipe) {
    for(i = i + 1; i < nbOption; i ++) {
      commandPipe[j] = malloc(strlen(command[i]) + 1);
      strcpy(commandPipe[j], command[i]);
      j++;
      command[i - 1] = NULL;
    }
    command[i - 1] =NULL;
    execCommandPipe(command, commandPipe); // command avec pipe

  }
  else {
    if(TARPATH != NULL)
      commandTar(command);
    else if(commandPersonnalisee(command) == -1) //command perso sans pipe
       execCommand(command); // command sans le pipe
  }

  return; // 0 if has no pipe, and 1 if has pipe
}

int commandPersonnalisee(char ** command) {
  int nbCommand = 2;
  char * commandPerso[nbCommand];
  int numeroCommand = -1;
  commandPerso[0] = "exit";
  commandPerso[1] = "cd";

  for (size_t i = 0; i < nbCommand; i++) {
    if(!strcmp(commandPerso[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -1;
    case 0 : exit(0);
    case 1 :
      cdPerso(command[1]);
      break;
  }
  return 0;
}

int commandTar(char ** command) {
  int nbCommand = 9;
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
  cmdTar[8] = "exit";

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
      return 0;
    case 1 :
      // return navigateTar(command[1]);
      return navigate(command[1]);
    case 8 :
      exit(0);
  }
  return -1;

}

int estTar(char * token) { // verifie si un token est un .tar
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

// prends un path et verifie si il y a un tar dans le path
// fonction qui appelle hasTar = cdPerso
int hasTar(char * path){
  char * token;
  int i = 0;
    //copie du path
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

// fonction pere = commandTar
int navigate(char * path){// ..
  char * fullpath[100];
  char * token;
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));

  int i = 0;
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    if(!strcmp(token,"..")){

      if(i == 0) dotdot(tmp);
      else{
        free(fullpath[i-1]);

        i--;
      }
    }
    else { // si c'est pas ..
      printf("token pour fullpath : %s\n", token);
      fullpath[i] = malloc(strlen(token)+1);
      memcpy(fullpath[i],token,strlen(token));
      strcat(fullpath[i],"/");
      i++;
    }

  }
  if(i == 0)return 0;

  //transformation du tableau de string("java") en chaine de caractere
  for(int x = 1;x<i;x++){
    strcat(fullpath[0],fullpath[x]);
    printf("fullpath[0] : %s\n", fullpath[0]);
  }

  char * tmp2 = malloc(sizeof(char)+1);
  char * tarp = malloc(strlen(TARPATH)+1);
  memcpy(tarp,TARPATH,strlen(TARPATH));// tarp = TARPATH copie

  token = strtok_r(tarp,"/",&tarp);
  printf("tarp :%s\n", tarp);

  // if(!strcmp(token, tarp)){
  //   printf("fullpath %s:\n", fullpath[0]);
  //
  //   return checkPath(fullpath[0], token);
  // }
  if(tarp != NULL){
    memcpy(tmp2,tarp,strlen(tarp));
    strcat(tmp2, "/");
    printf("tmp2 copie de tarp :%s\n", tmp2);
    strcat(tmp2,fullpath[0]);
    printf("tmp2 strcat fullpath :%s\n", tmp2);
    return checkPath(tmp2, token); // token toujours le fichier.tar
  }
  return checkPath(fullpath[0], token);

}

int checkPath(char * path, char * token){
  int file, n;
  if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}

  struct posix_header * p = malloc(sizeof(struct posix_header));

  while((n = read(file,p,BLOCKSIZE))>0){
    if((p->name[strlen(p->name) -1]=='/' && !strncmp(p->name, path, strlen(p->name)-1)) || !strcmp(p->name, path)){

      // if(TARPATH[strlen(TARPATH-1)]!='/') strcat(TARPATH, "/");
      if(token[strlen(token) -1] != '/')
        strcat(token, "/");
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
      return 0;
    }
    lseek(file,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
  }
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
  printf("Avant le while\n");
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    printf("Dans le while\n");

    if(tmp == NULL){// Pas SUR


      if(strlen(tmp2) != 0){
        printf("tmp2 :%s\n", tmp2);
        TARPATH = NULL;
        TARPATH = malloc(strlen(tmp2));
        strncpy(TARPATH,tmp2, strlen(tmp2) - 1);
        printf("tarpath apres copy tmp2 : %s\n", TARPATH);
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
int cdPerso(char * path){

  if(!hasTar(path)){ // si dans le path il y un tar
    if(TARPATH==NULL){
        printf("cdPerso path :%s\n", path);
       char * tmp = cd(path);
      if(tmp != NULL) {// en gros si path est du genre fichier.tar
        TARPATH = malloc(strlen(tmp) + 1);
        strcpy(TARPATH, tmp);
        return 0;
      }
      return -1;
    }
    else{
      return 0;
    }
  }
  chdir(path);
  return 0;
}


void * cd (char * path) { //ex: path = leTest.tar
  char * basicPath = malloc(sizeof(char)+1);
  char * token;
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));
  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
    if(!estTar(token)){
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
