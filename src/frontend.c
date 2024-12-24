#include "frontend.h"

#include <stdlib.h>

#include "board.h"

frontend_t* frontend_new(void) {
    frontend_t* env = malloc(sizeof(struct frontend_t));
    env->games = gamelist_new();
    env->spec = tagspec_new();
    env->symbols = symboltable_new();
    env->board = board_new();

    return env;
}

void frontend_free(frontend_t* env) {
    gamelist_free(env->games);
    tagspec_free(env->spec);
    symboltable_free(env->symbols);
    board_free(env->board);

    free(env);
}
