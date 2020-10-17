#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int typeFic(struct stat st);
int rights(struct stat st);
int Nbrlink(struct stat st);
int usrAndGrp(struct stat st);
int psize(struct stat st);
int mtime(struct stat st);
int ls(char * buff);
