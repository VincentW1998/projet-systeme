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

int hasTar(char * pathError);

int estTar(char * token);

void * findTar(char * path);

int cdPerso(char * path);

void findPipeAndExec(int nbOption, char ** command, char ** commandPipe);

int execCommandPipe(char ** command, char ** commandPipe);

void * cd (char * path);
