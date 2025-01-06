#include "movegen.h"

#include <stdbool.h>

#include "board.h"
#include "move.h"
#include "square.h"

bool insert_move(movelist_t* moves, board_t* board, square_t from,
                 square_t to) {
    if (from == SQUARE_OOB || to == SQUARE_OOB) {
        return false;
    }

    move_t* move = move_new(from, to);
    move->capture = board->squares[to] != SQUARE_EMPTY;

    movelist_insert_end(moves, move);

    return !move->capture;
}

movelist_t* movegen_bishop(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank + i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank - i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank + i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank - i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    return moves;
}

movelist_t* movegen_king(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    insert_move(moves, board, from, square_from(file, rank + 1));
    insert_move(moves, board, from, square_from(file, rank - 1));
    insert_move(moves, board, from, square_from(file + 1, rank));
    insert_move(moves, board, from, square_from(file + 1, rank + 1));
    insert_move(moves, board, from, square_from(file + 1, rank - 1));
    insert_move(moves, board, from, square_from(file - 1, rank));
    insert_move(moves, board, from, square_from(file - 1, rank + 1));
    insert_move(moves, board, from, square_from(file - 1, rank - 1));

    return moves;
}

movelist_t* movegen_pawn(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    square_t up = square_from(file, rank + 1);
    if (up != SQUARE_OOB && board->squares[up] == SQUARE_EMPTY) {
        insert_move(moves, board, from, up);
    }

    square_t left = square_from(file - 1, rank + 1);
    if (left != SQUARE_OOB && board->squares[left] == SQUARE_EMPTY) {
        insert_move(moves, board, from, left);
    }

    square_t right = square_from(file + 1, rank);
    if (right != SQUARE_OOB && board->squares[right] == SQUARE_EMPTY) {
        insert_move(moves, board, from, right);
    }

    return moves;
}

movelist_t* movegen_knight(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    insert_move(moves, board, from, square_from(rank + 1, file + 2));
    insert_move(moves, board, from, square_from(rank + 1, file - 2));
    insert_move(moves, board, from, square_from(rank - 1, file + 2));
    insert_move(moves, board, from, square_from(rank - 1, file - 2));

    insert_move(moves, board, from, square_from(rank + 2, file + 1));
    insert_move(moves, board, from, square_from(rank + 2, file - 1));
    insert_move(moves, board, from, square_from(rank - 2, file + 1));
    insert_move(moves, board, from, square_from(rank - 2, file - 1));

    return moves;
}

movelist_t* movegen_queen(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank + i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank - i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank + i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank - i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, rank + i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, rank - i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    return moves;
}

movelist_t* movegen_rook(board_t* board, square_t from) {
    movelist_t* moves = movelist_new();

    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = rank + 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = rank - 1; i >= 0; i--) {
        square_t to = square_from(file, i);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = file + 1; i < SQUARE_FILE_COUNT; i++) {
        square_t to = square_from(i, rank);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    for (int i = file - 1; i >= 0; i--) {
        square_t to = square_from(i, rank);
        if (!insert_move(moves, board, from, to)) {
            break;
        }
    }

    return moves;
}
