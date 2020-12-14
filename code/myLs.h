//
//  myLs.h
//  shell
//
//  Created by Félix Ngo on 30/11/2020.
//

#ifndef myLs_h
#define myLs_h
#include "tar.h"

int LsWithoutPath(void);

int lsRep(char * path);

void simpleLs(void);

int ls(int nbOptions, char ** path);

void printOccurences(void);

int validPath(char * path, char * target);

int checkpath(char * path);
#endif /* myLs_h */
