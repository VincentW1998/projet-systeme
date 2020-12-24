#ifndef redirection_h
#define redirection_h

#include <stdio.h>
int filePipe [2];
char * tarRedirectedDestination;
char * fileToBeDeleted;

int isDirectory(char * lastToken);

void flush(void);

void stopRedirection(void);

int nbOptionRedirect(int nbOption, char ** cmd);

void storeTarDestination(char * lastToken);

void moveToTarDest(void);

int redirectTar(char * lastToken);

int redirect(char * lastToken);

int redirection(char * path);
#endif /* redirection_h */
