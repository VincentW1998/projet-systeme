#ifndef GESTIONNAIRE_H
#define GESTIONNAIRE_H
#define BUFSIZE 512

#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
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
#include <time.h>

char * TARPATH;

int affichagePrompt(void);

int execCommand(char ** command);

int separateurCommand(char * buff, char ** command);

void findPipeAndExec(int nbOption, char ** command);

int commandShell(int nbOption, char ** command);

int commandTar(int nbOption, char ** command);

int estTar(char * token);

int existTar(char * token);

int hasTar(char * pathError);

char * findTar(char * path);

char * pathFromTar(char * path);

char * getPathBeforeTar(char * path);

char * getLastToken(char * path);

char * pathWithoutLastToken(char * path, char * lastToken);

char * substringTar(void);

char * subWithoutTar(void);

char * createPath(const char * path);

char * createPathFile(const char * path);

int commandNoTar(char * cmd, char * path);

int commandNoTar_option(char * cmd, char *opt, char * path);

void setTarpath(char * tarp);

int displayError(char * msg);
#endif
