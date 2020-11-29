#include "gestionnaire.h"
#include "my_cd.h"
#include "my_cat.h"

int affichagePrompt() { // affichage du prompt
  write(1, KBLU, strlen(KBLU));
  write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
  if(*TARPATH != '\0'){ // si TARPATH non vide
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

// erreur perso

int printerror(char * cmd, char * msg){
	write(2, msg, strlen(msg));
	perror(cmd);
	return -1;
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

// pour les commandes externes du shell avec un pipe
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
    if(*TARPATH != '\0')
      commandTar(nbOption, command);
    else if(commandPersonnalisee(nbOption, command) == -1) //command perso sans pipe
       execCommand(command); // command sans le pipe
  }

  return; // 0 if has no pipe, and 1 if has pipe
}

int commandPersonnalisee(int nbOption , char ** command) {
  int nbCommand = 3;
  char * commandPerso[nbCommand];
  int numeroCommand = -1;
  commandPerso[0] = "exit";
  commandPerso[1] = "cd";
	commandPerso[2] = "cat";

  for (size_t i = 0; i < nbCommand; i++) {
    if(!strcmp(commandPerso[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -1;
		  
    case 0 : exit(0);
		  
    case 1 : if(nbOption == 1) return cdNoOptions();
			 return cdPerso(command[1]);
		  
	case 2 : return cat(nbOption, command);
  } 
  return 0;
}

int commandTar(int nbOption, char ** command) {
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
      // printf("Here\n");
      write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
      write(1, "/", 1);
      write(1, TARPATH, strlen(TARPATH));
      write(1, "\n", 2);
      return 0;
    case 1 :
      if(nbOption == 1)
        return cdNoOptions();
      return navigate(command[1]);
		  
	case 6:
		  return cat(nbOption,command);
    case 8 :
      exit(0);
  }
  return -1;

}

int estTar(char * token) { // verifie si un token est un .tar
//  printf("estTAR\n");
  char * temp = malloc(strlen(token)+1);
  memcpy(temp,token,strlen(token));
  char * tok = strtok_r(temp, ".",&temp);
  char * name = NULL;

  while((tok = strtok_r(temp, ".\n",&temp)) != NULL) {
    name = malloc(strlen(tok) + 1);
    strcpy(name, tok);
  }
  if(name != NULL && !strcmp(name, "tar")){
    // free(temp);
    return 0;
  }

  // free(temp);
  return -1;
}

int existTar(char * token){
	char * tar = malloc(strlen(token) + 1);
	strcpy(tar, token);
  DIR * dir = opendir(".");
  struct dirent * cur;

  while((cur = readdir(dir)) > 0){
	  if(strcmp(cur->d_name,tar) == 0){
		  closedir(dir);
		  return 0;
		  
	  }
  }
  closedir(dir);
  write(2,"no such file or directory:\n",strlen("no such file or directory:\n"));
  perror("error: ");
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
//	strcpy(tmp,path);

    while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
      if(!estTar(token)){
//         free(tmp);
        return 0;
      }
      i++;
    }
//     free(tmp);
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

//int checkPath(char * path, char * token, int typeflag){
//  printf("dans le Check\n");
////  printf("token :%s\n", token);
////  printf("%s!\n", path);
//  int file, n;
//  if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}
//
//  struct posix_header * p = malloc(sizeof(struct posix_header));
//  while((n = read(file,p,BLOCKSIZE))>0){
////    printf("%s!\n",p->name );
//    if(((typeflag == 5) && (p-> typeflag == '5') && (!strncmp(path, p -> name, strlen(path)))) ){
//      size_t length = strlen(path);
//      strcpy(TARPATH,"\0");//problem
//      if(path[strlen(path)-1]=='/') length--;
//      TARPATH = realloc(TARPATH, strlen(token) + length + 1);
//      strcpy(TARPATH,token);
//      strncat(TARPATH,"/",strlen("/"));
//      strncat(TARPATH, path, length);
//      free(token);
//      close(file);
//      return 0;
//    }
//    else if(typeflag == 0 && p-> typeflag == '0' && !strcmp(p->name, path)){
//      free(token);
//      close(file);
//      return 0;
//     }
//    lseek(file,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
//  }
//  free(token);
//  close(file);
//  return -1;
//}
