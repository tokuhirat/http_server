#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void fatal(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
