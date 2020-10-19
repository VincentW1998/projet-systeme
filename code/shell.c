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


int affichagePrompt() {
  write(1, KBLU, strlen(KBLU));
  write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
  write(1, "> ", 2);
  write(1, RESET, strlen(RESET));
  return 0;
}

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char ** test;
  char * buff, * buff2, * token;
  int n;
  int nbrOption = 0;
  int i = 1;
  char * command[100];

  while(1) { // boucle infinie
    affichagePrompt();
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée

      token = strtok(str,"\n");
      if(token!=NULL){
        buff = malloc(strlen(token) + 1);
        strcpy(buff,token);
        strcat(buff,"\n");
      }

      buff2 = malloc(strlen(buff) + 1);
      strcpy(buff2, buff);

      token = strtok(buff, " \n");
      command[0] = malloc(strlen(token) + 1);
      nbrOption++;
      strcpy(command[0], token);
      while((token = strtok(NULL, " \n")) != NULL) {
        command[i] = malloc(strlen(token) + 1);
        strcpy(command[i], token);
        i ++;
        nbrOption++;
      }

     command[i] = NULL;
     command[i+1] = NULL;
     pid_t pid = fork();
     if(pid != 0) {
       wait(NULL);
     }
     else {
       execvp(command[0], command);
     }

      if(!strncmp(buff2,"exit\n",strlen("exit\n")))
        break;
      // implementation ls
      // if(!strncmp(buff,"ls",strlen("ls")))
      //   ls(buff);
      //
      // if(!strncmp(buff, "mkdir", strlen("mkdir"))) {
      //
      //   token = strtok(buff, " \n");
      //   token = strtok(NULL, " \n");
      //
      //   my_mkdir(token);
      // }
      // if(!strncmp(buff, "rmdir", strlen("rmdir"))) {
      //   token = strtok(buff, " \n");
      //   token = strtok(NULL, " \n");
      //
      //
      //   my_rmdir(token);
      // }
      if(!strncmp(buff2, "cd\n", strlen("cd\n"))) {
        token = strtok(buff2, " \n");
        token = strtok(NULL, " \n");

        chdir(token);
      }
      if(!strncmp(buff2, "pwd\n", strlen("pwd\n"))) {
        write(1, getcwd(NULL, 0), strlen(getcwd(NULL, 0)));
        write(1, "\n\n", 2);
      }
      for (int j = 0; j < nbrOption; j++){
        free(command[j]);
      }
    }
    strcpy(buff2,"");
  }
  return 0;
}
