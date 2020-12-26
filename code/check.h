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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/random.h>

struct posix_header newHd;
int rmOn; // si tu utilises rm -> 1 sinon 0
char * pathFileTarget; // name of target file
char * tarTarget; //name of tar of target file
int endFile; // premier block vide du fichier target
int cpOn;
int rmdirOn; // if we are using rmdir command -> 1 else 0
int found;
char buf[512];

void next_header(int fd, unsigned int filesize);

int read_header(int fd, char *path);

int checkEntete(char * tarName, char * path);

int read_header_r(int fd, char *path);

int checkEntete_r(char * tarName, char * path);

int decalage(int fd, int pos);

int fin(int fd, int pos);

int hasPosixHeader(int fd);

int hasRmdirOn(int fd, int filesize);

int hasCpOn(int fd, int filesize);

int hasRm_r(int fd, int filesize);
#endif
