#include "board_move.h"

#include <stdlib.h>

board_movelist_t *board_movelist_new(void) {
    board_movelist_t *board_moves = malloc(sizeof(board_movelist_t));

    board_moves->head = NULL;
    board_moves->tail = NULL;

    return board_moves;
}

void board_move_free(board_move_t *board_move) {
    while (board_move != NULL) {
        board_move_t *next = board_move->next;

        free(board_move);

        board_move = next;
    }
}

void board_movelist_free(board_movelist_t *board_moves) {
    if (board_moves == NULL) {
        return;
    }

    board_move_free(board_moves->head);

    free(board_moves);
}
