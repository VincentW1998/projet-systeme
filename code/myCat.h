#ifndef MYCAT_H
#define MYCAT_H
#include "tar.h"

int showContent(int file, int filesize);

int simpleCat(char * path);

int CatFile (char * path);

int readFile(char * path, char * tar);

int cat(int nbOption, char ** path);

#endif
