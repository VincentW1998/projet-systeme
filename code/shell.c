#include "gestionnaire.h"
#include "pipe.h"

int main(int argc, char const *argv[]) {
  // buffer qui va nous permettre de recuperer la ligne de l'utilisateur
  char str [BUFSIZE];
  // initialisation du buffer
  memset(str, '\0', BUFSIZE);

  int n;
  char * command[100];
  memset(command, '\0', 100 * sizeof(command[0]));
  int nbOption;
  TARPATH = malloc(1);
  memset(TARPATH, '\0', 1);
  
  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée

      // separe buff en command, option, path dans une variable command[]
      nbOption = separateurCommand(str, command);
      memset(str, '\0', BUFSIZE);

      findPipeAndExec(nbOption, command);
      // on remet à null le tableau qui prenait les differentes commandes
      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }
    }
  }
  return 0;
}
