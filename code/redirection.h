#ifndef redirection_h
#define redirection_h

#include <stdio.h>
int filePipe [2];

int isDirectory(char * lastToken);

int redirection(char * path);
#endif /* redirection_h */
