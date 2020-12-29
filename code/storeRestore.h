#ifndef STORERESTORE_H
#define STORERESTORE_H
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char * pos, * posTar;

void storePosition();

void restorePosition();

char * getPos();

char * getTARPATH();

void storePosition2(char * tp, char * ps);
#endif
