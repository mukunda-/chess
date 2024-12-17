#include "app.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "game.h"
#include "generation.h"
#include "pgn.lex.h"
#include "pgn.syntax.h"
#include "strutil.h"
#include "tagspec.h"

#define UNUSED(x) (void)(x)

void run(frontend_t *env) {
    print_headers(env->spec);

    yyscan_t scanner;
    yylex_init(&scanner);
    // Parse into the games list.
    yyparse(scanner, env);
    yylex_destroy(scanner);
}

void flush_games(frontend_t *env) {
    for (game_t *game = env->games->head; game != NULL && game->result != NULL;
         game = game->next) {
        print_game(env->spec, game);
    }

    gamelist_free(env->games);
    env->games = gamelist_new();
}

void yyerror(YYLTYPE *yyllocp, yyscan_t scanner, frontend_t *env,
             const char *msg) {
    UNUSED(scanner);
    UNUSED(env);
    UNUSED(yyllocp);
    fprintf(stderr, "error: %s\n", msg);
}
