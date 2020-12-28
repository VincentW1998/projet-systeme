#include "pipe.h"
#include "gestionnaire.h"


// return number of pipe in the command line
int nbPipes(int nbOption, char ** command) {
  int nPipe = 0;
  for(int i = 0; i< nbOption; i++) {
    if(strcmp(command[i], "|") == 0)
      nPipe += 1;
  }
  return nPipe;
}



// pour les commandes externes du shell avec un pipe
int execCommandPipe(char ** command, char ** commandPipe) {
  int fd[2];
  if(pipe(fd) < -1){
    perror("pipe");
  }
  int n, w;
  pid_t pid1;

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
      exit(1);
      break;
    default :
      pid1 = fork();

      switch (pid1) {
        case -1 :
          perror("fork");
          break;

        case 0 : // fils, command, ecrivain, fd[1]
          close(fd[0]);
          dup2(fd[1], 1);
          close(fd[1]);
          if((n = execvp(command[0], command)) == -1)
            exit(1);
          exit(1);
          break;
      }
      close(fd[0]);
      close(fd[1]);
      wait(&w); //attend le processus fils
//      waitpid(pid1, &w, 0);
  }
  return 0;
}


