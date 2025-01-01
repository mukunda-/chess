#include "pgn.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "gameclock.h"
#include "pgn_move.h"
#include "pgn_tag.h"

pgn_t *pgn_new(void) {
    struct pgn_t *pgn = malloc(sizeof(struct pgn_t));

    pgn->result = NULL;
    pgn->tags = taglist_new();
    pgn->moves = pgn_movelist_new();

    pgn->clock_white = gameclock_new();
    pgn->clock_black = gameclock_new();

    pgn->ply = 0;

    pgn->next = NULL;

    return pgn;
}

void pgn_free(struct pgn_t *pgn) {
    while (pgn != NULL) {
        pgn_t *next = pgn->next;
        pgn_movelist_free(pgn->moves);
        taglist_free(pgn->tags);
        gameclock_free(pgn->clock_white);
        gameclock_free(pgn->clock_black);
        free(pgn->result);
        free(pgn);

        pgn = next;
    }
}

pgnlist_t *pgnlist_new(void) {
    pgnlist_t *pgns = malloc(sizeof(pgnlist_t));

    pgns->head = pgn_new();
    pgns->tail = pgns->head;

    return pgns;
}

void pgnlist_free(pgnlist_t *pgns) {
    if (pgns == NULL) {
        return;
    }

    pgn_free(pgns->head);

    free(pgns);
}

void pgnlist_add_result(pgnlist_t *pgns, const char *result) {
    pgns->tail->result = strdup(result);
    pgns->tail->next = pgn_new();
    pgns->tail = pgns->tail->next;
}
