#include "gestionnaire.h"
#include "storeRestore.h"

void storePosition() {
  pos = getcwd(NULL, 0);
  posTar = malloc(strlen(TARPATH) + 1);
  strcpy(posTar, TARPATH);
}

void restorePosition() {
  chdir(pos);
  TARPATH = realloc(TARPATH, strlen(posTar) + 1);
  strcpy(TARPATH, posTar);
}

char * getTARPATH(){
  return TARPATH;
}

char * getPos(){
  return getcwd(NULL,0);
}

void storePosition2(char * tp, char * ps){
  pos = malloc(strlen(ps)+1);
  strcpy(pos,ps);
  posTar = malloc(strlen(tp)+1);
  strcpy(posTar,tp);
}
