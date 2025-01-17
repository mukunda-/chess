#include "pgn_frontend.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "generation.h"
#include "pgn.h"
#include "pgn.lex.h"
#include "pgn.syntax.h"
#include "symbol.h"

#define UNUSED(x) (void)(x)

pgn_frontend_t *pgn_frontend_new(void) {
    pgn_frontend_t* env = (pgn_frontend_t*)malloc(sizeof(struct pgn_frontend_t));
    assert(env != NULL && "Out of memory");
    memset(env, 0, sizeof(pgn_frontend_t));

    env->pgns = pgnlist_new();
    env->spec = tagspec_new();
    env->symbols = symboltable_new();
    env->board = board_new(WHITE);

    return env;
}

void pgn_frontend_free(pgn_frontend_t *env) {
    pgnlist_free(env->pgns);
    tagspec_free(env->spec);
    symboltable_free(env->symbols);
    board_free(env->board);

    free(env);
}

void pgn_frontend_run(pgn_frontend_t *env) {
    print_headers(env->spec);

    yyscan_t scanner;
    yylex_init(&scanner);
    // Parse into the pgns list.
    yyparse(scanner, env);
    yylex_destroy(scanner);
}

void flush_pgns(pgn_frontend_t *env) {
    for (pgn_t *pgn = env->pgns->head; pgn != NULL && pgn->result != NULL;
         pgn = pgn->next) {
        print_pgn(env->spec, pgn);
    }

    pgnlist_free(env->pgns);
    env->pgns = pgnlist_new();

    symboltable_free(env->symbols);
    env->symbols = symboltable_new();
}

void yyerror(YYLTYPE *yyllocp, yyscan_t scanner, pgn_frontend_t *env,
             const char *msg) {
    UNUSED(scanner);
    UNUSED(env);
    UNUSED(yyllocp);
    fprintf(stderr, "error: %s\n", msg);
}
