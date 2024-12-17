#include "frontend.h"

#include "stdlib.h"

frontend_t* frontend_new() {
    frontend_t* env = malloc(sizeof(struct frontend_t));
    env->games = gamelist_new();
    env->spec = tagspec_new();
    env->symbols = symboltable_new();

    return env;
}

void frontend_free(frontend_t* env) {
    gamelist_free(env->games);
    tagspec_free(env->spec);
    symboltable_free(env->symbols);

    free(env);
}
