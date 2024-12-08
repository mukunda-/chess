#ifndef FRONTEND_H
#define FRONTEND_H

#include "gamelist.h"
#include "symbols.h"
#include "tagspec.h"

typedef struct frontend_t {
    gamelist_t* games;
    tagspec_t* spec;
    symbols_t* symbols;
} frontend_t;

/* Allocates a new frontend */
frontend_t* frontend_new();

/* Deallocates frontend */
void frontend_free(frontend_t* env);

#endif
