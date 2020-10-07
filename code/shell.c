#define BUFSIZE 512
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include<string.h>

int main(int argc, char const *argv[]) {
  char buff [BUFSIZE];
  int n, file;
  while(1){ // boucle infinie
    if((n = read(0,buff,BUFSIZE)) > 0){ // check si l'usr ecrit dans l'entrée
      // implementation de la commande exit
      if(!strncmp(buff,"exit\n",strlen("exit\n")))
        break;

    }
  }
  return 0;
}
