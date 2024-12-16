#ifndef APP_H
#define APP_H

#include <stddef.h>
#include <stdio.h>

#include "frontend.h"
#include "tagspec.h"

typedef void *yyscan_t;

void run(frontend_t *env);

#endif
