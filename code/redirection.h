#ifndef redirection_h
#define redirection_h

#include <stdio.h>
int fpipe [2];
int saveDup;

int isDirectory(char * lastToken);

void flush(void);

void stopRedirection(void);

int nbOptionRedirect(int nbOption, char ** cmd);

void storeTarDestination(char * lastToken);

//void moveToTarDest(void);

void setFileToBeDeleted(void);

int redirectTar(char * lastToken);

int redirect(char * lastToken);

int redirection(char * path);
#endif /* redirection_h */
