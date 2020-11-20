#ifndef MYCD_H
#define MYCD_H

int moveTo(char * path, char * tarball);

int cdNoOptions();

int navigate(char * path);

// int checkPath(char * path, char * token);

int dotdot(char * path);

int cdPerso(char * path);

int cd (char * path);
int checkPath(char * path, char * token, int typeflag);
#endif
