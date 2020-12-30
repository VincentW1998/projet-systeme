#include "myCp.h"
#include "monrm.h"

char * monMv(int nbOption, char ** command, int dos){
  if (nbOption<3) return -1;
  char ** c [nbOption-1];
  if (dos == 1 && nbOption >= 4){
    c[1] = malloc(strlen("-r")+1);
    for(int i = 2; i<nbOption-1;i++){
      c[i] = malloc(strlen(command[i])+1);
      memset(c[i],'\0',strlen(command[i])+1);
      strcpy(c[i],command[i]);
    }
    
    for(int i = 2 ; i < nbOption-1; i++){
      cpTar(nbOption,c);
      rmTar(nbOption,c);
    }
  }
  else {
    for(int i = 1; i<nbOption -1 ;i++){
      c[i] = malloc(strlen(command[i])+1);
      memset(c[i],'\0',strlen(command[i])+1);
      strcpy(c[i],command[i]);
    }
    for(int i = 1 ; i < nbOption-1;i++){
      cpTar(nbOption,c);
      rmTar(nbOption,c);
    }
  }
  return 1;
}
