#ifndef MYMKDIR_H
#define MYMKDIR_H
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

char * pos, * posTar;

struct posix_header newHeader(const char * path);

int mkdirTar(int nbOption, char **path);

char * createPathForMkdir(const char * path);

int createRepo(char * path);
#endif
