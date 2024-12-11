#ifndef APP_H
#define APP_H

#include <stddef.h>
#include <stdio.h>

#include "frontend.h"
#include "tagspec.h"

typedef void *yyscan_t;
void initialize_tagspec(tagspec_t *spec, FILE *roster_fp);
void run(frontend_t *env);
void print_headers(frontend_t *env);

#endif
