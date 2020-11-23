//#include "tar.h"
int isAbsolute(char * path);

int showContent(int file, struct posix_header * p);

int CatFile (char * path);

int checkpath(char * path);

int cat(int nbOption, char ** path);
