#define BUFSIZE 512
#include "my_ls.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include<string.h>
#include <dirent.h>

int main(int argc, char const *argv[]) {
  char buff [BUFSIZE];
  DIR * current;
  current = opendir(".");
  int n, file;
  while(1){ // boucle infinie
    if((n = read(0,buff,BUFSIZE)) > 0){ // check si l'usr ecrit dans l'entrée
      // implementation de la commande exit
      if(!strncmp(buff,"exit\n",strlen("exit\n")))
        break;
      if(!strncmp(buff,"ls",strlen("ls")))
        ls(buff, current);


    }
  }
  closedir(current);
  return 0;
}
