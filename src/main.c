#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "gamelist.h"
#include "movelist.h"
#include "pgn.tab.h"
#include "strutil.h"
#include "taglist.h"
#include "tagspec.h"

typedef void *yyscan_t;
int yylex_init(yyscan_t *yyscanner);
int yylex_destroy(yyscan_t yyscanner);

void initialize_tagspec(tagspec_t *spec, FILE *roster_fp);
void run(frontend_t *env);
void print_headers(frontend_t *env);

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

    frontend_t *env = frontend_new();

    initialize_tagspec(env->spec, roster_fp);
    fclose(roster_fp);

    print_headers(env);

    run(env);

    frontend_free(env);

    return EXIT_SUCCESS;
}

void run(frontend_t *env) {
    yyscan_t scanner;
    yylex_init(&scanner);
    // Parse into the games list.
    yyparse(scanner, env);
    yylex_destroy(scanner);
}

void initialize_tagspec(tagspec_t *spec, FILE *roster_fp) {
    char *line = NULL;
    size_t line_len;
    while (getline(&line, &line_len, roster_fp) != -1) {
        trim_right(line);
        tagspec_add(spec, line, "", TAG_ALWAYS);
    }

    free(line);
}

void print_headers(frontend_t *env) {
    for (tagcmp_t *cmp = env->spec->head; cmp != NULL; cmp = cmp->next) {
        printf("%s\t", cmp->name);
    }
    printf("moves");
    printf("\tresult");
    printf("\tply");

    printf("\n");
}

void print_moves(move_t *moves_head) {
    int ply = 0;
    for (move_t *move = moves_head; move != NULL; move = move->next) {
        if (move->kind == MOVE_TYPE_MOVE) {
            if (ply++ > 0) {
                printf(" ");
            }

            printf("%s", move->value);
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
    printf("%s\t", game->result);

    // print ply
    printf("%d\t", game->ply);

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
