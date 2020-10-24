#define BUFSIZE 512
// #include "tar.h"
#define KBLU  "\x1B[34m"
#define RESET "\x1B[0m"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>


char * TARPATH;
int CHOIX;

int affichagePrompt();

int afficheMessageErreur(char ** command);

int execCommand(char ** command);

void *lectureLigne(char * str, char * buff);

int separateurCommand(char * buff, char ** command);

int commandPersonnalisee(char ** command);

int commandTar(char ** command);

int cdPerso(char * path);

// int navigateTar(char * path);

int navigate(char * path);

int dotdot(char * path);

int checkPath(char * path, char * token);

int hasTar(char * command);

int estTar(char * token);

void * cd(char * path);
// void * cd (int nbOption, char ** command);

// void * findTar(int nbOption, char ** command);
