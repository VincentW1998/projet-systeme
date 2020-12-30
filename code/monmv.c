#include <string.h>
#include <stdlib.h>
#include "monmv.h"

int monMv(int nbOption, char ** command, int dos){
  if (nbOption<3) return -1;
  char * c [nbOption+1];
  char * c1 [nbOption-1];
  char * c2 [nbOption];
  if (dos == 1){
    c[1] = malloc(strlen("-r")+1);
    strcpy(c[1],"-r");

    c2[1] = malloc(strlen("-r")+1);
    strcpy(c2[1],"-r");
    
    for(int i = 1; i<nbOption;i++){
      c[i+1] = malloc(strlen(command[i])+1);
      memset(c[i+1],'\0',strlen(command[i])+1);
      strcpy(c[i+1],command[i]);
    }

    for(int i = 1; i<nbOption-1; i++){
      c2[i+1] = malloc(strlen(command[i])+1);
      memset(c2[i+1],'\0',strlen(command[i])+1);
      strcpy(c2[i+1],command[i]);
    }
    cpTar(nbOption+1,c);
    rmTar(nbOption,c2);
  }
  else {
    for(int i = 1; i<nbOption -1 ;i++){
      c1[i] = malloc(strlen(command[i])+1);
      memset(c1[i],'\0',strlen(command[i])+1);
      strcpy(c1[i],command[i]);
    }
    cpTar(nbOption,command);
    rmTar(nbOption-1,c1);
  }
  return 1;
}
