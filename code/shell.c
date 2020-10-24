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
      buff = lectureLigne(str, buff); // readline

      // separe buff en command, option, path dans une variable command[]
      nbOption = separateurCommand(buff, command);

      //verifions si dans le path il y a un pipe |
      // et en fonction s'il y a un pipe ou pas
      // execute execCommandPipe, commandPersonnalisee ou execCommand
      findPipeAndExec(nbOption, command, commandPipe);

      // si la variable tarpath est initialisé alors on utilise une commande
      // pour les tarball
      if(TARPATH != NULL)
        CHOIX = commandTar(command);


      // on remet à null le tableau qui prenait les differentes commandes
      for (size_t j = 0; j < nbOption; j++) {
        command[j] = NULL;
      }

    }
  }
  return 0;
}
