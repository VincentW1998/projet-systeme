#include "gestionnaire.h"

int main(int argc, char const *argv[]) {
  // buffer qui va nous permettre de recuperer la ligne de l'utilisateur
  char str [BUFSIZE];
  // initialisation du buffer
  memset(str, '\0', BUFSIZE);
  char * buff;
  int n;
  char * command[100];
  memset(command, '\0', 100 * sizeof(command[0]));
  char * commandPipe[100];
  memset(commandPipe, '\0', 100 * sizeof(commandPipe[0]));
  int nbOption = 0;
  TARPATH = malloc(1);
//  strcpy(TARPATH,"\0");
  memset(TARPATH, '\0', 1);
  
  while(1) { // boucle infinie
    affichagePrompt();
    nbOption = 0;
    if((n = read(0,str,BUFSIZE) > 0)) { // check si l'usr ecrit dans l'entrée
      buff = lectureLigne(str, buff); // readline
      
      // separe buff en command, option, path dans une variable command[]
      nbOption = separateurCommand(buff, command);
      memset(buff, '\0', strlen(buff) + 1);
      
      findPipeAndExec(nbOption, command, commandPipe);
      
      // on remet à null le tableau qui prenait les differentes commandes
      for (size_t j = 0; j < nbOption; j++) {
        //command[j] = NULL;
        free(command[j]);
      }
      
      
    }
  }
  return 0;
}
