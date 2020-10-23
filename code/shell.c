#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff, * buff2, * token;
  int n, i = 1;
  char * command[100];
  char * commandPipe[100];
  int nbOption = 0;
  char *pathFictif;
  // char *tarPath;
  int longueurPath;
  int pipe;

  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée
      CHOIX = -1;
      buff = lectureLigne(str, buff);


      // separe buff en command, option, path dans une variable command[]
      nbOption = separateurCommand(buff, command);

      //verifions si dans le path il y a un pipe |

      pipe = findPipe(nbOption, command, commandPipe); // 0 if has not pipe else 1


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
        if (pipe) {
          execCommandPipe(command, commandPipe);
        }
        execCommand(command);
      }

      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }


  }
  return 0;
}
