#include "strutil.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void trim_right(char *str) {
    char *tail = str + strlen(str) - 1;

    while (isspace(*tail) && tail >= str) {
        *tail = '\0';
        tail--;
    }
}
