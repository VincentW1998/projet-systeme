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

void restoreManually(char * position){
	char * bfTar = getPathBeforeTar(position);
	char * fromTar = pathFromTar(position);
	chdir(bfTar);
	TARPATH = realloc(TARPATH, strlen(fromTar) + 1);
	strcpy(TARPATH, fromTar);
}

char * storeManually(){
	char * current = getcwd(NULL, 0);
	size_t len = strlen(current) + strlen(TARPATH) + 2;
	char * position = malloc(len);
	memset(position, '\0', len);
	strcpy(position, current);
	if(TARPATH[0] != '\0'){
		strcat(position,"/");
		strcat(position,TARPATH);
	}
	return position;
}
