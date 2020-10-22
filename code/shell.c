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

  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée
      CHOIX = -1;
      buff = lectureLigne(str, buff);

      // creation de buff2 car buff va etre 'ecraser' par strtok
      buff2 = malloc(strlen(buff) + 1);
      strcpy(buff2, buff);

      // separe buff en command, option, path
      nbOption = separateurCommand(buff, command);
      char * tmp = findTar(nbOption, command);

      if(tmp != NULL) {
        TARPATH = malloc(strlen(tmp) + 1);
        strcpy(TARPATH, tmp);
      }

      printf("printf : TARPATH : %s\n", TARPATH);
      if(TARPATH != NULL)
        CHOIX = commandTar(command);

      if(CHOIX != 0)
        CHOIX = commandPersonnalisee(command);
      if(CHOIX == -2){
        execCommand(command);
      }

      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }

    // strcpy(buff2,"");
  }
  return 0;
}
