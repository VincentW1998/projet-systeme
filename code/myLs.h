
#ifndef myLs_h
#define myLs_h
#include "tar.h"

int LsWithoutPath(int withL);

int lsRep(char * path, int withL);

void simpleLs(int withL);

int ls(int nbOptions, char ** path);

void printOccurences(int withL);

int validPath(char * path, char * target);

int checkpath(char * path);

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
