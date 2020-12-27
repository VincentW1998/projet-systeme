#ifndef MONRM_H
#define MONRM_H
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/random.h>

int contient(char *dossier,char *nom);

//int rmfichier_tar(int fichier,char *chemindossier,char *cheminarchive,char *c    );

int rmfichier_tar(char * path);

//int rm_r_tar(int fichier,char *chemindossier,char *cheminarchive,char *c);

int rmTar(int nbOption, char ** command);

#endif
