#ifndef STORERESTORE_H
#define STORERESTORE_H
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char * pos, * posTar;

void storePosition(void);

void restorePosition(void);

void restoreManually(char * position);

char * storeManually(void);
#endif
