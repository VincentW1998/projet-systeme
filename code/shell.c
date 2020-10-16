#define BUFSIZE 512
#include <stdlib.h>
#include "my_ls.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "my_mkdir.h"
#include "my_rmdir.h"

int main(int argc, char const *argv[]) {
  char buff [BUFSIZE];
  DIR * current;
  current = opendir(".");
  int n, file, lu;
  char * token;
  while(1) { // boucle infinie
    if((n = read(0,buff,BUFSIZE)) > 0) { // check si l'usr ecrit dans l'entrée
      // implementation de la commande exit

      if(!strncmp(buff,"exit\n",strlen("exit\n")))
        break;
      // implementation ls
      if(!strncmp(buff,"ls",strlen("ls")))
        ls(buff, current);
      if(!strncmp(buff, "mkdir", strlen("mkdir"))) {

        token = strtok(buff, " \n");
        token = strtok(NULL, " \n");

        my_mkdir(token);
      }
      if(!strncmp(buff, "rmdir", strlen("rmdir"))) {
        token = strtok(buff, " \n");
        token = strtok(NULL, " \n");

        my_rmdir(token);
      }
    }
  }
  closedir(current);
  return 0;
}
