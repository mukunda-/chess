#ifndef PGN_H
#define PGN_H

#include "gameclock.h"
#include "pgn_move.h"
#include "pgn_tag.h"

/**
 * A game
 */
typedef struct pgn_t {
    char *result;
    taglist_t *tags;
    pgn_movelist_t *moves;
    gameclock_t *clock_white;
    gameclock_t *clock_black;
    struct pgn_t *next;
    int ply;
} pgn_t;

/**
 * A list of pgns.
 */
typedef struct pgnlist_t {
    pgn_t *head;
    pgn_t *tail;
} pgnlist_t;

/* Allocate a pgnlist */
pgnlist_t *pgnlist_new(void);

/* Dealocate memory from all the pgns */
void pgnlist_free(pgnlist_t *pgns);

/*
 * Sets the result of the current game.
 */
void pgnlist_add_result(pgnlist_t *pgns, const char *result);

#endif
