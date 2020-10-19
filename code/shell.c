#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff, * buff2, * token;
  int n, i = 1;
  char * command[100];
  int nbOption = 0;
  int choix, choixBis;

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
