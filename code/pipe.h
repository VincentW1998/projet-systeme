#ifndef PIPE_H
#define PIPE_H
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>


int nbPipes(int nbOption, char ** command);

char ** separateurPipe(int nbOption, char ** command, char ** cmdPipe);


void pipeCommand(char ** cmdPipe, int nbPipes);


//int execCommandPipe(char ** command, char ** commandPipe);

#endif

