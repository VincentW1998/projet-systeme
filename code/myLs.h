
#ifndef myLs_h
#define myLs_h
#include "tar.h"

int withL;

int manageOption(int nbOptions, char ** path);

int LsWithoutPath(void);

int lsRep(char * path);

void simpleLs(void);

int ls(int nbOptions, char ** path);

void printOccurences(void);

int validPath(char * path, char * target);

int checkpath(char * path);

int whichValid(char * tar, char * name);

//optionL functions
void optionL (struct posix_header * p, int file);

void typeFic(struct posix_header * p);

void rights(struct posix_header * p);

void nbrlink(struct posix_header * p, int file);

void usrAndGrp(struct posix_header * p);

void psize(struct posix_header * p);

void mtime(struct posix_header * p);

long octalConverter (char * octal);

int countLinks(char * name ,int file);
#endif /* myLs_h */
