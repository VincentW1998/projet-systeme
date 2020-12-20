#include "gestionnaire.h"
#include "myCd.h"
#include "myCat.h"
#include "myMkdir.h"
#include "myLs.h"

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
    if(*TARPATH != '\0')
      commandTar(nbOption, command);
    else if(commandPersonnalisee(nbOption, command) == -1) //command perso sans pipe
       execCommand(command); // command sans le pipe
  }

  return; // 0 if has no pipe, and 1 if has pipe
}

int commandPersonnalisee(int nbOption , char ** command) {
	int nbCommand = 4;
	char * commandPerso[nbCommand];
	int numeroCommand = -1;
	commandPerso[0] = "exit";
	commandPerso[1] = "cd";
	commandPerso[2] = "cat";
	commandPerso[3] = "ls";
	
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

  for (int i = 0; i < nbCommand; i++) {
    if(!strcmp(cmdTar[i], command[0]))
      numeroCommand = i;
  }
  printf("nbOption : %d\n", nbOption);
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
		  
	case 3 : return mkdirTar("dos.tar", "DossierB");
		  
	case 6:  return cat(nbOption,command);
		  
	case 8 : exit(0);
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

void * findTar(char * path){
  char * tmp = malloc(strlen(path)+1);
  memcpy(tmp,path,strlen(path));
  char * token;
  while((token = strtok_r(tmp, "/\n", &tmp)) != NULL)
    if(!estTar(token)) return token;
  return NULL;
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

void returnToPos(char * pos, char * posTar){
	chdir(pos);
	TARPATH = realloc(TARPATH, strlen(posTar) + 1);
	if(strlen(posTar) == 0) 
		strcpy(TARPATH,"");
	else strcpy(TARPATH, posTar);
}
