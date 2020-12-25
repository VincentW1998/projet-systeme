#ifndef MYCP_H
#define MYCP_H
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int cpTar(int nbOption, char ** command);

int cpRepo(char * path, char * target);

#endif
