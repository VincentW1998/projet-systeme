#ifndef PIPE_H
#define PIPE_H
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>


int execCommandPipe(char ** command, char ** commandPipe);


#endif

