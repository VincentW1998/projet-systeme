#define BUFSIZE 512
#define KBLU  "\x1B[34m"
#define RESET "\x1B[0m"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>

int affichagePrompt();

int afficheMessageErreur(char ** command);

int execCommand(char ** command);

void *lectureLigne(char * token, char * str, char * buff);

int separateurCommand(char * token, char * buff, char ** command);

int commandPersonnalisee(char ** command);
