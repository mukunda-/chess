#include "game.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "gameclock.h"
#include "move.h"
#include "tag.h"

game_t *game_new(void) {
    struct game_t *game = malloc(sizeof(struct game_t));

    game->result = NULL;
    game->tags = taglist_new();
    game->moves = movelist_new();

    game->clock_white = gameclock_new();
    game->clock_black = gameclock_new();

    game->ply = 0;

    game->next = NULL;

    return game;
}

void game_free(struct game_t *game) {
    if (game == NULL) {
        return;
    }

    movelist_free(game->moves);
    taglist_free(game->tags);
    game_free(game->next);
    gameclock_free(game->clock_white);
    gameclock_free(game->clock_black);

    free(game->result);

    free(game);
}

gamelist_t *gamelist_new(void) {
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
