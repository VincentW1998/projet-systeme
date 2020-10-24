#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  char str [BUFSIZE];
  char * buff, * buff2, * token;
  int n, i = 1;
  char * command[100];
  char * commandPipe[100];
  int nbOption = 0;
  char *pathFictif;
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

      // si la variable tarpath est initialisé alors on utilise une commande
      // pour les tarball
      if(TARPATH != NULL)
        CHOIX = commandTar(command);

      // si aucune commande tar à ete effectué, testons si on peut executer
      // une commande personalisée (cd, (pwd), exit)
      if(CHOIX != 0)
        CHOIX = commandPersonnalisee(command);

      // si aucune commande personalisée à ete effectué
      if(CHOIX == -2){
        // execute une command pour les pipe
        if (pipe) {
          execCommandPipe(command, commandPipe);
        }
        // sans les pipe
        else
          execCommand(command);
      }

      // on remet à null le tableau qui prenait les differentes commandes
      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }
  }
  return 0;
}
