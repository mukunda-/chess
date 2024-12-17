#ifndef FRONTEND_H
#define FRONTEND_H

#include "game.h"
#include "symbol.h"
#include "tagspec.h"

typedef struct frontend_t {
    gamelist_t* games;
    tagspec_t* spec;
    symboltable_t* symbols;
} frontend_t;

/* Allocates a new frontend */
frontend_t* frontend_new(void);

/* Deallocates frontend */
void frontend_free(frontend_t* env);

#endif
