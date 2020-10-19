#define BUFSIZE 512
#define KBLU  "\x1B[34m"
#define RESET "\x1B[0m"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
// #include "my_ls.h"
// #include "my_mkdir.h"
// #include "my_rmdir.h"

int affichagePrompt() { // affichage du prompt
  write(1, KBLU, strlen(KBLU));
  write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
  write(1, "> ", 2);
  write(1, RESET, strlen(RESET));
  return 0;
}


int execCommand(char ** command) {
  pid_t pid = fork();
  if(pid != 0) { // on attend que le processus fils se lance
    wait(NULL);
  }
  else { // puis on execute la commande souhaité
    execvp(command[0], command);
    return 0;
  }
  return -1;
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
    write(1, "\n\n", 2);
    break;
  }
  return 0;
}



int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff, * buff2, * token;
  int n, i = 1;
  char * command[100];
  int nbOption = 0;
  int choix;

  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée

      buff = lectureLigne(token, str, buff);

      // creation de buff2 car buff va etre 'ecraser' par strtok
      buff2 = malloc(strlen(buff) + 1);
      strcpy(buff2, buff);

      // separe buff en command, option, path
      nbOption = separateurCommand(token, buff, command);
      choix = commandPersonnalisee(command);
      if(choix == -1){
        execCommand(command);
      }

      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }

    strcpy(buff2,"");
  }
  return 0;
}
