#ifndef GAME_H
#define GAME_H

#include "movelist.h"
#include "taglist.h"

/**
 * A game
 */
typedef struct game_t {
    char *result;
    taglist_t *tags;
    movelist_t *moves;
    struct game_t *next;
    int ply;
} game_t;

/**
 * A list of games.
 */
typedef struct gamelist_t {
    game_t *head;
    game_t *tail;
} gamelist_t;

/* Allocate a gamelist */
gamelist_t *gamelist_new();

/* Dealocate memory from all the games */
void gamelist_free(gamelist_t *games);

/*
 * Sets the result of the current game.
 */
void gamelist_add_result(gamelist_t *games, const char *result);

#endif
