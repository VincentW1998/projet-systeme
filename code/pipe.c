#include "pipe.h"
#include "gestionnaire.h"


// return number of pipe in the command line
int nbPipes(int nbOption, char ** command) {
  int number = 0;
  for (int i = 0; i < nbOption; i++) {
    if(strcmp(command[i], "|") == 0) {
      number ++;
    }
  }
  return number;
}


/* example : 
 command {"ls", "|", "grep .o", "|", "wc}
 this function return 
 cmdPipe = {"ls", "grep .o", "wc"} 
 */
char ** separateurPipe(int nbOption, char ** command, char ** cmdPipe) {
  int length = 0; // lenght for malloc
  int j = 0; // index of cmdPipe
  char * tmp = malloc(sizeof(char));
  memset(tmp, '\0', 1);
  int espace = 0; // help to know if we should add space " " or not

  for(int i = 0; i< nbOption; i++) {

    if(strcmp(command[i], "|") != 0) { // if element != "|"
      if(espace) { // need space
        length += strlen(command[i]);
        tmp = realloc(tmp, length + 2);
        strcat(tmp, " ");
        strcat(tmp, command[i]);
      }
      else { // don't need space
        length += strlen(command[i]);
        tmp = realloc(tmp, length + 1);
        strcat(tmp, command[i]);
        espace = 1;
      }
          }
    else{ // element == "|"
      length = 0;
      espace = 0;
      cmdPipe[j] = malloc(strlen(tmp) + 1);
      strcpy(cmdPipe[j], tmp);
      j ++;
      memset(tmp, '\0', strlen(tmp));
    }
  }
  // add the last tmp 
  cmdPipe[j] = malloc(strlen(tmp) + 1);
  strcpy(cmdPipe[j], tmp);
  memset(tmp, '\0', strlen(tmp));
  return cmdPipe;
}
/* exec pipe fonction */
void pipeCommand(char ** cmdPipe, int nbPipes) {
  int fd[2];
  int lastFd = 0;
  int nOption;
  pid_t pid;
  char * cmd[100];
  memset(cmd, '\0', 100 * sizeof(cmd[0]));

  for(int i = 0; i < nbPipes; i++) {

    nOption = separateurCommand(cmdPipe[i], cmd);

    if(pipe(fd) < -1) {
      return;
    }

    pid = fork();
    switch(pid) {
      case -1 : exit(1);

      case  0 : //fils, lecteur, fd[0]
        dup2(lastFd, 0);
        if((i + 1) < nbPipes) // existe une commande suivante
          dup2(fd[1], 1);
        close(fd[0]);

        // exec the command
        if(TARPATH[0] != '\0') {
          if (commandTar(nOption, cmd) == -1)
            execCommand(cmd);
        }
        else if(commandPersonnalisee(nOption, cmd) == 0)
          execCommand(cmd);
        exit(1);

      default : // attend le fils
        wait(NULL);
        close(fd[1]);
        lastFd = fd[0];
    }
  }
}





