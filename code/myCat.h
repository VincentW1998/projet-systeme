#ifndef MYCAT_H
#define MYCAT_H

void goToInitialPos();

int showContent(int file, struct posix_header * p);

int simpleCat(char * path);

int CatFile (char * path);

int checkpath(char * path);

int cat(int nbOption, char ** path);

#endif
