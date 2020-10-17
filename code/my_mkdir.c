#include <stdio.h>
#include "my_mkdir.h"

int my_mkdir (const char * path) {
    int n;
    if((n = mkdir(path, 0777) > -1)){
        return 0;
    }
    return -1;
}

