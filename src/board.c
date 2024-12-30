#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

board_t *board_new(void) {
    board_t *board = malloc(sizeof(board_t));
    for (size_t i = 0; i < BOARD_SQUARE_COUNT; i++) {
        board->squares[i] = BOARD_PIECE_EMPTY;
    }

    board->turn = WHITE;

    return board;
}

void board_free(board_t *board) { free(board); }

void board_set_piece(board_t *board, board_file_t file, board_rank_t rank,
                     board_piece_t piece) {
    board->squares[file + (rank * BOARD_RANK_COUNT)] = piece;
}

board_rank_t board_get_rank(board_t *board, board_piece_t piece,
                            board_file_t file) {
    for (int rank = 0; rank < BOARD_RANK_COUNT; rank++) {
        if (board->squares[file + (rank * BOARD_RANK_COUNT)] == piece) {
            return rank;
        }
    }

    return BOARD_RANK_OFF_BOARD;
}

board_file_t board_get_file(board_t *board, board_piece_t piece,
                            board_rank_t rank) {
    for (int file = 0; file < BOARD_FILE_COUNT; file++) {
        if (board->squares[file + (rank * BOARD_RANK_COUNT)] == piece) {
            return file;
        }
    }

    return BOARD_FILE_OFF_BOARD;
}

board_piece_t board_get_piece(board_t *board, board_file_t file,
                              board_rank_t rank) {
    return board->squares[file + (rank * BOARD_RANK_COUNT)];
}

void board_move(void) {
    /*
     *
     * <Piece> ::= 'N' | 'B' | 'R' | 'Q' | 'K'
     * <Rank> ::= '0' .. '8'
     * <File> ::= 'a' .. 'h'
     * <Square> ::= <File><Rank>
     * <From> ::= <File> | <Rank> | <Square>
     * <To> ::= Square
     * <Capture> :: = 'x'
     * <Piece moves> ::= <Piece><From>[<Capture>]<To>
     * <Pawn moves> ::= <File>[<Rank>]<Capture><To>[<Promotion>]
     * <Pawn capture> ::= <To>[<Promotion>]
     */
}
