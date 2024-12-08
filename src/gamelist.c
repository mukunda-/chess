#include "gamelist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "movelist.h"
#include "taglist.h"

struct game_t *game_new() {
    struct game_t *game = malloc(sizeof(struct game_t));

    game->result = NULL;
    game->tags = taglist_new();
    game->moves = movelist_new();
    game->next = NULL;
    game->ply = 0;

    return game;
}

void game_free(struct game_t *game) {
    if (game == NULL) {
        return;
    }

    movelist_free(game->moves);
    taglist_free(game->tags);
    game_free(game->next);

    free(game->result);

    free(game);
}

gamelist_t *gamelist_new() {
    gamelist_t *games = malloc(sizeof(gamelist_t));

    games->head = game_new();
    games->tail = games->head;

    return games;
}

void gamelist_free(gamelist_t *games) {
    if (games == NULL) {
        return;
    }

    game_free(games->head);

    free(games);
}

void gamelist_add_result(gamelist_t *games, const char *result) {
    games->tail->result = strdup(result);
    games->tail->next = game_new();
    games->tail = games->tail->next;
}
