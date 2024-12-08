#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "gamelist.h"
#include "movelist.h"
#include "pgn.tab.h"
#include "strutil.h"
#include "symbols.h"
#include "taglist.h"
#include "tagspec.h"

typedef void *yyscan_t;
int yyparse(yyscan_t yyscanner, frontend_t *env);
int yylex_init(yyscan_t *yyscanner);
int yylex_destroy(yyscan_t yyscanner);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tag roster>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file in read mode, exit with error if failure to do so
    const char *roster_path = argv[1];
    FILE *roster_fp = fopen(roster_path, "r");
    if (roster_fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    frontend_t env;
    env.games = gamelist_new();
    env.spec = tagspec_new();
    env.symbols = symbols_new();

    char *line = NULL;
    size_t line_len;
    while (getline(&line, &line_len, roster_fp) != -1) {
        trim_right(line);
        tagspec_add(env.spec, line, "", TAG_ALWAYS);
    }

    free(line);
    fclose(roster_fp);

    // Print TSV Header row
    for (tagcmp_t *cmp = env.spec->head; cmp != NULL; cmp = cmp->next) {
        printf("%s\t", cmp->name);
    }
    printf("moves\tresult");
    printf("\n");

    {
        yyscan_t scanner;
        yylex_init(&scanner);
        // Parse into the games list.
        yyparse(scanner, &env);
        yylex_destroy(scanner);
    }

    gamelist_free(env.games);
    tagspec_free(env.spec);
    symbols_free(env.symbols);

    return EXIT_SUCCESS;
}

void print_moves(move_t *moves_head) {
    for (move_t *move = moves_head; move != NULL; move = move->next) {
        if (move->kind == MOVE_TYPE_MOVE) {
            printf("%s ", move->value);
        }
    }
}

void print_tags(taglist_t *tags, tagspec_t *spec) {
    taglist_t *aligned_tags = taglist_new_aligned(tags, spec);
    for (tag_t *tag = aligned_tags->head; tag != NULL; tag = tag->next) {
        printf("%s\t", tag->value);
    }

    taglist_free(aligned_tags);
}

void print_game(frontend_t *frontend, game_t *game) {
    print_tags(game->tags, frontend->spec);

    // Print the move list
    print_moves(game->moves->head);
    printf("\t");

    // Print result
    printf("%s", game->result);

    printf("\n");
}

void flush_games(frontend_t *env) {
    for (game_t *game = env->games->head; game != NULL && game->result != NULL;
         game = game->next) {
        print_game(env, game);
    }

    gamelist_free(env->games);
    env->games = gamelist_new();
}

void yyerror(YYLTYPE *yyllocp, yyscan_t unused, frontend_t *env,
             const char *msg) {
    fprintf(stderr, "error: %s\n", msg);
}
