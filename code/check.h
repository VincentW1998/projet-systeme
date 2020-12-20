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

int found;
char buf[512];

void next_header(int fd, unsigned int filesize);

int read_header(int fd, char *path);

int read_header2(int fd, char *path, struct posix_header * newHd);

int checkEntete(char * tarName, char * path);

int checkEntete2(char * tarName, char * path, struct posix_header * hd);

#endif
