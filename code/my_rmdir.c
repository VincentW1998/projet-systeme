#include <stdio.h>
#include "my_rmdir.h"

int my_rmdir(const char * path) {
    int n;
    if((n = rmdir(path) > -1)) {
        return 0;
    }
    return -1;
}
