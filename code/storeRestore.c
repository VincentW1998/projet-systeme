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
