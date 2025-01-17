#include "board.h"

board_t *board_new(board_turn_t turn) {
    board_t* board = (board_t*)malloc(sizeof(board_t));
    assert(board != NULL && "Out of memory");
    memset(board, 0, sizeof(board_t));
    
    for (size_t i = 0; i < SQUARE_COUNT; i++) {
        board->squares[i] = SQUARE_EMPTY;
    }

    board->turn = turn;

    return board;
}

void board_free(board_t *board) { free(board); }

void board_set_piece(board_t *board, square_t square, square_piece_t piece) {
    board->squares[square] = piece;
}

square_rank_t board_get_rank(board_t *board, square_piece_t piece,
                             square_file_t file) {
    for (int rank = 0; rank < SQUARE_RANK_COUNT; rank++) {
        if (board->squares[square_from(file, rank)] == piece) {
            return rank;
        }
    }

    return SQUARE_RANK_OOB;
}

square_file_t board_get_file(board_t *board, square_piece_t piece,
                             square_rank_t rank) {
    for (int file = 0; file < SQUARE_FILE_COUNT; file++) {
        if (board->squares[square_from(file, rank)] == piece) {
            return file;
        }
    }

    return SQUARE_FILE_OOB;
}

square_piece_t board_get_piece(board_t *board, square_t square) {
    return board->squares[square];
}
