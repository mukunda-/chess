#include "movegen.h"

#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "square.h"

bool insert_move(movelist_t* moves, board_t* board, square_t from,
                 square_t to) {
    move_t* move = move_new(from, to);
    move->capture = board->squares[to] != SQUARE_EMPTY;

    movelist_insert_end(moves, move);

    return !move->capture;
}

movelist_t* movegen_rook(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int r = rank + 1; r < SQUARE_RANK_COUNT; r++) {
        square_t to = square_from(file, r);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    return moves;
}
