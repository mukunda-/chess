#include "pgn_move.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

pgn_movelist_t *pgn_movelist_new(void) {
    pgn_movelist_t *moves = malloc(sizeof(pgn_movelist_t));

    moves->head = NULL;
    moves->tail = NULL;

    return moves;
}

void pgn_movelist_add(pgn_movelist_t *moves, pgn_movetype_t kind,
                      const char *value) {
    pgn_move_t *move = malloc(sizeof(pgn_move_t));

    move->next = NULL;
    move->kind = kind;
    move->value = strdup(value);

    if (moves->head == NULL) {
        moves->head = move;
        moves->tail = move;
    } else {
        moves->tail->next = move;
        moves->tail = moves->tail->next;
    }
}

void pgn_movelist_free(pgn_movelist_t *moves) {
    if (moves == NULL) {
        return;
    }

    pgn_move_t *head = moves->head;

    while (head != NULL) {
        pgn_move_t *next = head->next;

        free(head->value);
        free(head);

        head = next;
    }

    free(moves);
}
