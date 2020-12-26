#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff;
  int n;
  char * command[100];
  char * commandPipe[100];
  int nbOption = 0;
  TARPATH = malloc(1);
  strcpy(TARPATH,"\0");
  
  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée
      buff = lectureLigne(str, buff); // readline
      
      // separe buff en command, option, path dans une variable command[]
      nbOption = separateurCommand(buff, command);
      
      findPipeAndExec(nbOption, command, commandPipe);
      
      // on remet à null le tableau qui prenait les differentes commandes
      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }
      
    }
  }
  return 0;
}
