#include "gestionnaire.h"

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

void *lectureLigne(char * token, char * str, char * buff){ //readLine
  token = strtok(str,"\n");
  if(token!=NULL){
    buff = malloc(strlen(token) + 1);
    strcpy(buff,token);
    strcat(buff,"\n");
  }
  return buff;
}

int separateurCommand(char * token, char * buff, char ** command){ // separe la ligne en tableau de char
  token = strtok(buff, " \n");
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
    case -1 : return -1;
    case 0 : exit(0);
    case 1 :
      chdir(command[1]);
      break;
    default :
    write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
    write(1, "\n", 2);
    break;
  }
  return 0;
}

int estTar(char * token) {
  char * tok = strtok(token, ".");
  char * name;
  while((tok = strtok(NULL, ".\n")) != NULL) {
    name = malloc(strlen(tok) + 1);
    strcpy(name, tok);
  }
  if(!strcmp(name, "tar"))
    return 0;
  return -1;
}


void * findTar(char *token, int nbOption, char ** command) {
  char * pathFictif;
  char * token2;
  for (size_t i = 1; i < nbOption; i++){
    char * tmp = malloc(strlen(command[i]) + 1);
    strcpy(tmp, command[i]);
    token = strtok(tmp, "/\n");
    char * tmp2 = malloc(strlen(token) + 1);
    strcpy(tmp2, token);
    if(!estTar(tmp2)){
      // printf("token : %s\n", token);

      return token;
    }


    while((token2 = strtok(NULL, "/\n")) != NULL) {
      char * tmp = malloc(strlen(token2) + 1);
      if(estTar(tmp)){
        // printf("token2 : %s\n", token2);
        return token2;

      }
    }

  }
  return NULL;
}
