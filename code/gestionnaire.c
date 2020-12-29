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
#include "pipe.h"

/**************************** AFFICHAGE ****************************/
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
  memset(erreurBuf, '\0', strlen(erreur) + strlen(command[0]) + 2);
  strcpy(erreurBuf, erreur);
  strcat(erreurBuf, command[0]);
  strcat(erreurBuf, "\n");
  write(2, erreurBuf, strlen(erreurBuf));
  free(erreurBuf);
  return 0;
}


/**************************** LECTURE LIGNE ****************************/
//fonction qui se comporte comme readLine
/*void *lectureLigne(char * str, char * buff){
  char * token = malloc(strlen(str));
  memset(token, '\0', strlen(str));
  token = strtok_r(str, "\n", &str);
  if(token != NULL) {
    buff = malloc(strlen(token) + 1);
    strcpy(buff, token);
  }
  memset(str, '\0', strlen(str));
  return buff;
}*/

// separe la ligne en tableau de char
int separateurCommand(char * buff, char ** command){
  char * token = NULL;
  int nbOption = 0;
  int l = strlen(buff);

  while((token = strtok_r(buff, " \n", &buff)) != NULL) {
    command[nbOption] = malloc(strlen(token) + 1);
    strcpy(command[nbOption], token);
    nbOption ++;
  }
  if (nbOption == 0) {
    command[nbOption] = malloc(1);
    strcpy(command[nbOption], "");
    nbOption ++;
  }
  memset(buff, '\0', l);
  command[nbOption] = NULL;
  nbOption = nbOptionRedirect(nbOption, command); //redirect
  return nbOption;
}


/*********************** APPEL DES COMMANDES ***********************/

void whichCommand(int nbOption, char ** command) {
  if(TARPATH[0] != '\0')
   commandTar(nbOption, command);
  else if(commandPersonnalisee(nbOption, command) == 0)
   execCommand(command);
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
      if((n = execvp(command[0], command)) == -1) {
          afficheMessageErreur(command);
          exit(1);
      }
      break;
    default :
    if(strcmp(command[0], "cat") == 0)
    signal(SIGINT, SIG_IGN);
      wait(&w);
      signal(SIGINT, SIG_DFL);
  }
  return 0;
}

/* si la ligne de commande contient au moins un pipe
 * on appelle la fonction pipeCommand du fichier pipe.c
 * sinon on execute normalement nos commandes avec au choix
 * commandTar : les commandes du tar
 * commandPersonalisee : commande du shell
 * execCommand : utilise la fonction exec
*/
void findPipeAndExec(int nbOption, char ** command) {
  int nPipe = nbPipes(nbOption, command); // return number pipe
  if(nPipe != 0) {
    char * cmdPipe[nPipe + 1];
    memset(cmdPipe, '\0', (nPipe + 1) * sizeof(cmdPipe[0]));
    separateurPipe(nbOption, command, cmdPipe);
    pipeCommand(cmdPipe, nPipe + 1);
    return;
  }
  /*else {
    if(*TARPATH != '\0')
      commandTar(nbOption, command);

    //command perso sans pipe
    else if(commandPersonnalisee(nbOption, command) == 0) 
      execCommand(command); // command sans le pipe
  } */
    else
      whichCommand(nbOption, command);
  stopRedirection(); // redirect
  return;
}

int commandPersonnalisee(int nbOption , char ** command) {
  int nbCommand = 10;
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
  commandPerso[9] = "";
  
  for (int i = 0; i < nbCommand; i++) {
    if(!strcmp(commandPerso[i], command[0]))
      numeroCommand = i;
  }
  switch (numeroCommand) {
    case -1 : return 0; //renvoie 0 si la commande ne figure pas dans le tableau
      
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

    case 9 : return 1;

  }
  return 1;
}

int commandTar(int nbOption, char ** command) {
  int nbCommand = 11;
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
  cmdTar[10] = "";

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

  case 10 : return 1;
  }
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
      return 1;
    }
  }
  closedir(dir);
  write(2,"no such file or directory:\n",strlen("no such file or directory:\n"));
  perror("error: ");
  return -1;
}

int estTar(char * token) { // verifie si un token est un .tar
	char * tmp = malloc(strlen(token) +1 );
	strcpy(tmp, token);
	char * tok = strtok_r(tmp,"/",&tok);
	//	free(tmp);
	if(hasTar(tok) == 1) return 1;
	return -1;
}



// prends un path et verifie si il y a un tar dans le path
// fonction qui appelle hasTar = cdPerso
int hasTar(char * path){
  char * token;
  if( (token = strstr(path,".tar/")) !=NULL) return 1;
  if( ((token = strstr(path,".tar"))!=NULL) && (strcmp(token,".tar") == 0) ) return 1;
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

//return the path without the last Token
char * pathWithoutLastToken(char * path, char * lastToken){
  // copy path - size of the last token
  char * deplacement = malloc(strlen(path) - strlen(lastToken) + 1);
  memset(deplacement, '\0', strlen(path));
  strncpy(deplacement, path, strlen(path) - strlen(lastToken));
  return deplacement;
}

/* return the tar repository from TARPATH */
char * substringTar() {
  char *tmp = malloc(strlen(TARPATH) + 1);
  memset(tmp, '\0', strlen(TARPATH));
  strcpy(tmp, TARPATH);
  char * token;
  token = strtok_r(tmp, "/\n", &tmp);
  return token;
}

/* return string contains TARPATH without tar repository */
char * subWithoutTar() {
  char * tmp = malloc(strlen(TARPATH) + 1);
  memset(tmp, '\0', strlen(TARPATH));
  strcpy(tmp, TARPATH);
  strtok_r(tmp, "/\n", &tmp);
  return tmp;
}

// return lastToken in path
char * getLastToken(char * path) {
  char *tmp = malloc(strlen(path) + 1), * token;
  memset(tmp, '\0', strlen(path));
  strcpy(tmp, path);
  char * result = malloc(strlen(path) + 1);
  memset(result, '\0', strlen(path) + 1);
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
  size_t length = strlen(suiteName) + strlen(path) + 3;
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
  char * command [3] = {[0]=cmd,[1]=path};
  execCommand(command);
  return 1;
}


int commandNoTar_option(char * cmd, char *opt, char * path){
  char * command [4] = {[0]=cmd,[1]=opt,[2]=path};
  execCommand(command);
  return 1;
}
