#include "move.h"

#include <stdlib.h>

#include "square.h"

movelist_t* movelist_new(void) {
    movelist_t* moves = malloc(sizeof(movelist_t));
    moves->head = NULL;

    return moves;
}

void move_free(move_t* move) {
    if (move == NULL) {
        return;
    }

    move_free(move->next);
    free(move);
}

move_t* move_new(square_t from, square_t to) {
    move_t* move = malloc(sizeof(move_t));

    move->next = NULL;

    move->from = from;
    move->to = to;
    move->capture = false;

    return move;
}

void movelist_free(movelist_t* moves) {
    move_free(moves->head);
    free(moves);
}

void movelist_insert_end(movelist_t* moves, move_t* move) {
    if (moves->head == NULL) {
        moves->head = move;
    }

    if (moves->tail == NULL) {
        moves->tail = move;
    }

    moves->tail->next = move;
    moves->tail = move;
}
