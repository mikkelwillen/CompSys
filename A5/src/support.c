#include <stdio.h>
#include <stdlib.h>

#include "support.h"

void error(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(-1);
}
