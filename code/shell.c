#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff, * buff2, * token;
  int n, i = 1;
  char * command[100];
  int nbOption = 0;
  char *pathFictif;
  // char *tarPath;
  int longueurPath;
  TARPATH = NULL;

  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée
      CHOIX = -1;

      buff = lectureLigne(str, buff);
      // separe buff en command, option, path
      nbOption = separateurCommand(buff, command);

      if(TARPATH != NULL)
        CHOIX = commandTar(command);

      if(CHOIX != 0)
        CHOIX = commandPersonnalisee(command);
      printf("printf : TARPATH : %s\n", TARPATH);
      if(CHOIX == -2){
        execCommand(command);
      }

      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }

  }
  return 0;
}
