#ifndef MYMKDIR_H
#define MYMKDIR_H
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <uuid/uuid.h>

int mkdir(const char *path, mode_t mode);

int rmdir(const char *path);

int my_mkdir(const char *path);

struct posix_header newHeader(const char * path);

int mkdirTar(const char * pathTar, const char * path);
#endif
