#ifndef CHECK_H
#define CHECK_H
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include "tar.h"

struct posix_header newHd;
int rmOn; // si tu utilises rm -> 1 sinon 0
int rmdirOn; // if we are using rmdir command -> 1 else 0
int found;
char buf[512];

void next_header(int fd, unsigned int filesize);

int read_header(int fd, char *path);

int checkEntete(char * tarName, char * path);

int decalage(int fd, int pos);

int hasPosixHeader(int fd);

int hasRmdirOn(int fd);

int hasRmOn(int fd, int filesize);
#endif
