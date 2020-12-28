#ifndef MYCD_H
#define MYCD_H

int cdNoOptions(void);

int navigate(char * path);

int cdPerso(char * path);

int cd (char * path);

int checkPath(char * path, char * token);

//int checkfp(char *tar, char *fullpath[50], int i);

char * tabToString(int l, char ** fp);

int checkfp (char * tar, char * fp);

void setTarpath(char * tarp);


int cdAbs(char * path);

int whichCd(char * pathCd);
#endif
