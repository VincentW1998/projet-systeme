#include "gestionnaire.h"
#include "myCd.h"
#include "myCat.h"
#include "myMkdir.h"
#include "myLs.h"
#include "myRmdir.h"
#include "monrm.h"
#include "UnitTest.h"
#include "redirection.h"
#include "myCp.h"

//#include "tar.h"

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
    if(strcmp(command[0], "cat") == 0)
    signal(SIGINT, SIG_IGN);
      wait(&w);
			signal(SIGINT, SIG_DFL);
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
//    strcat(buff,"\n");
  }
  return buff;
}

// separe la ligne en tableau de char
int separateurCommand(char * buff, char ** command){
//  char * token = strtok_r(buff, " \n", &buff);
 // command[0] = malloc(strlen(token) + 1);
  //strcpy(command[0], token);
//  int i = 1;
  char * token = NULL;
  int i = 0;

  while((token = strtok_r(buff, " \n", &buff)) != NULL) {
    command[i] = malloc(strlen(token) + 1);
    strcpy(command[i], token);
    i ++;
  }
  int nbOption = i;
	nbOption = nbOptionRedirect(nbOption, command); //redirect
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
	stopRedirection(); // redirect
  return; // 0 if has no pipe, and 1 if has pipe
}

int commandPersonnalisee(int nbOption , char ** command) {
  int nbCommand = 9;
  char * commandPerso[nbCommand];
  int numeroCommand = -1;
  commandPerso[0] = "exit";
  commandPerso[1] = "cd";
  commandPerso[2] = "cat";
  commandPerso[3] = "ls";
  commandPerso[4] = "mkdir";
  commandPerso[5] = "rmdir";
	commandPerso[6] = "test";
  commandPerso[7] = "cp";
  commandPerso[8] = "rm";
	
  for (int i = 0; i < nbCommand; i++) {
    if(!strcmp(commandPerso[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -1;
      
    case 0 : exit(0);
      
    case 1 : if(nbOption == 1) return cdNoOptions();
           return cdPerso(command[1]);
      
    case 2 : return cat(nbOption, command);
      
    case 3 : return ls(nbOption, command);

    case 4 : return mkdirTar(nbOption, command);
		
		case 5 : return rmdirTar(nbOption, command);
			
		case 6 : return Test();

    case 7 : return cpTar(nbOption, command);

    case 8 : return rmTar(nbOption, command);

  }
  return 0;
}

int commandTar(int nbOption, char ** command) {
  int nbCommand = 10;
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
  cmdTar[9] = "rm";

  for (int i = 0; i < nbCommand; i++) {
    if(!strcmp(cmdTar[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return -1;

    case 0 : write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
         write(1, "/", 1);
         write(1, TARPATH, strlen(TARPATH));
         write(1, "\n", 2);
         return 0;
  case 1 : if(nbOption == 1) return cdNoOptions();
         return navigate(command[1]);
  
  case 2: return ls(nbOption, command);
      
  case 3 : return mkdirTar(nbOption, command);

  case 4 : return rmdirTar(nbOption, command);
      
//  case 5 : return mvJulien

  case 6:  return cat(nbOption,command);

  case 7 : return cpTar(nbOption, command);
      
  case 8 : exit(0);

  case 9 : return rmTar(nbOption, command);
  }
  return -1;

}

int estTar(char * token) { // verifie si un token est un .tar
  char * tmp = malloc(strlen(token) +1 );
  strcpy(tmp, token);
  char * tok = strtok_r(tmp,"/",&tok);
//	free(tmp);
  if(hasTar(tok) == 0) return 0;
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
  if( (token = strstr(path,".tar/")) !=NULL) return 0;
  if( ((token = strstr(path,".tar"))!=NULL) && (strcmp(token,".tar") == 0) ) return 0;
  return -1;
}

char * findTar(char * path){ // return the .tar filename
	if(hasTar(path) == -1) return NULL;
	char * tarp = pathFromTar(path);
	char * tmp = malloc(strlen(tarp) + 1);
	strcpy(tmp,tarp);
	char * tar;
	tar = strtok_r(tmp,"/",&tmp);
	return tar;
}
char * pathFromTar(char * path){ // return the path from the .tar
	if(hasTar(path) == -1) return NULL;
	char * tmp = malloc(strlen(path) + 1), *token;
	strcpy(tmp,path);
	size_t l = 0;
	if(path[0] == '/') l++;
	while((token = strtok_r(tmp, "/", &tmp)) != NULL){
		if(estTar(token) == 1) return path+l;
		l += 1 + strlen(token);
	}
	return NULL;
}

char * getPathBeforeTar(char * path){ // return the path before TARPATH
	if(hasTar(path) == -1) return NULL;
	char * fromTar = pathFromTar(path);
	char * beforeTar = malloc(strlen(path) - strlen(fromTar));
	strncpy(beforeTar,path,strlen(path) - strlen(fromTar)-1);
	return beforeTar;
}

char * getLastToken(char * path){ // return lastToken in path
	char * lastToken = malloc(1), * token;
	char * tmp = malloc( strlen(path) + 1); // copy of the path
	strcpy(tmp, path);
	while((token = strtok_r(tmp, "/",&tmp) )!= NULL){ //retrieve the next token that contains /
		lastToken = realloc(lastToken,strlen(token) + 1);
		strcpy(lastToken, token);
	}
	return lastToken;
}

//return the path without the last Token
char * pathWithoutLastToken(char * path, char * lastToken){
	// copy path - size of the last token
	char * deplacement = malloc(strlen(path) - strlen(lastToken) + 1);
	strncpy(deplacement, path, strlen(path) - strlen(lastToken));
	return deplacement;
}

/* return the tar repository from TARPATH */
char * substringTar() {
  char *tmp = malloc(strlen(TARPATH) + 1);
  strcpy(tmp, TARPATH);
  char * token;
  token = strtok_r(tmp, "/\n", &tmp);
  return token;
}

/* return string contains TARPATH without tar repository */
char * subWithoutTar() {
  char * tmp = malloc(strlen(TARPATH) + 1);
  strcpy(tmp, TARPATH);
  strtok_r(tmp, "/\n", &tmp);
  return tmp;
}


/* take a string path and return a substring of this path without the
 * last repository*/
char * subWithoutRepo(char * path) {
  char * token;
  char * tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);
  char * result = malloc(strlen(path) + 1);
  strcpy(result, "");
  if (path[0] == '/')
    strcat(result, "/");
  token = strtok_r(tmp, "/", &tmp);
  if(strlen(tmp) != 0)
    strcat(result, token);
  while((token = strtok_r(tmp, "/", &tmp)) != NULL ) {
    if(strlen(tmp) != 0) {
      strcat(result, "/");
      strcat(result, token);
    }
      }
  return result;
}

/* take a string path and return the last repository of this path*/
char * subWithRepo(char * path) {
  char *tmp = malloc(strlen(path) + 1);
  char * token;
  strcpy(tmp, path);
  char * result = malloc(strlen(path) + 1);
  strcpy(result, "");
  while((token = strtok_r(tmp, "/", &tmp)) != NULL) {
    if(strlen(tmp) == 0)
      strcat(result, token);
  }
  return result;
}


char * createPath(const char * path) {

  char * suiteName = subWithoutTar();

/* +3 car on rajoute 2 slash et il y a le caractere zero qui termine une
 * chaine de caracteres. */
  int length = strlen(suiteName) + strlen(path) + 3;
  char * pathWithFolder = malloc(length);
  pathWithFolder[0] = '\0';
  strncat(pathWithFolder, suiteName, strlen(suiteName));
  if (suiteName[0] != '\0') {
    strcat(pathWithFolder, "/");
  }
  strncat(pathWithFolder, path, strlen(path));
  strcat(pathWithFolder, "/");
  return pathWithFolder;
}

char * createPathFile(const char * path) {

  char * suiteName = subWithoutTar();

/* +3 car on rajoute 2 slash et il y a le caractere zero qui termine une
 * chaine de caracteres. */
  int length = strlen(suiteName) + strlen(path) + 2;
  char * pathWithFile = malloc(length);
  pathWithFile[0] = '\0';
  strncat(pathWithFile, suiteName, strlen(suiteName));
  if (suiteName[0] != '\0') {
    strcat(pathWithFile, "/");
  }
  strncat(pathWithFile, path, strlen(path));
  return pathWithFile;
}

int commandNoTar(char * cmd, char * path) {
  char * command [2] = {[0]=cmd,[1]=path};
  execCommand(command);
  return 1;
}


int commandNoTar_option(char * cmd, char *opt, char * path){
  char * command [4] = {[0]=cmd,[1]=opt,[2]=path};
  execCommand(command);
  return 1;
}
