#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "my_rmdir.h"

int my_rmdir(char * path) {
    int n;
    struct stat st;
    char *pwd;
    char *s = NULL;

    if(stat(path, &st) == -1) {
        perror("stat");
    }

    if(S_ISDIR(st.st_mode)){
        pwd = getcwd(NULL, 0);
        chdir(path); // change de directory
        char * token1 = strtok(path, "/"); // recupere le premier token
        char * token2;

        while((token2 = strtok(NULL, "/\n")) != NULL){ // recupere tout les autres token
          printf("token2 : %s\n", token2);
          s = malloc(strlen(token2) + 1);
          strcpy(s, token2);
        }

        chdir(".."); // retourne dans le dossier parent

        if(s == NULL) {
          free(s);
          s = malloc(strlen(token1) + 1);
          strcpy(s, token1);
        }
        printf("s : %s\n", s);
        printf("token : %s\n", token1);
        if((n = rmdir(s)) > -1) {
            free(s);
            chdir(pwd);
            free(pwd);
            return 0;
        }
        else {
          perror("rmdir");
        }
    }
    return -1;
}
