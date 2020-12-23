#ifndef MYCAT_H
#define MYCAT_H
#include "tar.h"

int showContent(int file, struct posix_header * p);

int simpleCat(char * path);

int CatFile (char * path);

int readFile(char * path, char * tar);

int cat(int nbOption, char ** path);

#endif
