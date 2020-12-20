#ifndef CHECK_H
#define CHECK_H

int found;
char buf[512];

void next_header(int fd, unsigned int filesize);

int read_header(int fd, char *path);

int checkEntete(char * tarName, char * path);


#endif
