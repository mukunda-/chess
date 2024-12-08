#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "strutil.h"

void trim_right(char *s) {
  char *tail = s + strlen(s) - 1;

  while (isspace(*tail) && tail >= s) {
    *tail = '\0';
  }
}
