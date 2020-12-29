#ifndef MYCD_H
#define MYCD_H

int cdNoOptions(void);

int navigate(char * path);

int cdPerso(char * path);

int cd (char * path);

char * tabToString(int l, char ** fp);

int checkfp (char * tar, char * fp);

void setTarpath(char * tarp);

int cdAbs(char * path);

int whichCd(char * pathCd);
#endif
