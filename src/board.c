#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

board_t *board_new(void) {
    board_t *board = malloc(sizeof(board_t));
    for (size_t i = 0; i < BOARD_RANK_COUNT; i++) {
        for (size_t j = 0; j < BOARD_FILE_COUNT; j++) {
            board->squares[i][j] = BOARD_PIECE_EMPTY;
        }
    }

    return board;
}

void board_free(board_t *board) { free(board); }

void board_print(board_t *board) {
    for (int i = 0; i < BOARD_FILE_COUNT; i++) {
        for (int j = 0; j < BOARD_RANK_COUNT; j++) {
            printf("%d ", board->squares[i][j]);
        }
        printf("\n");
    }
}

void board_set_piece(board_t *board, board_file_t file, board_rank_t rank,
                     board_piece_t piece) {
    board->squares[file][rank] = piece;
}

board_rank_t board_get_rank(board_t *board, board_piece_t piece,
                            board_file_t file) {
    for (int i = 0; i < BOARD_RANK_COUNT; i++) {
        if (board->squares[file][i] == piece) {
            return i;
        }
    }

    return BOARD_RANK_OFF_BOARD;
}

board_file_t board_get_file(board_t *board, board_piece_t piece,
                            board_rank_t rank) {
    for (int i = 0; i < BOARD_FILE_COUNT; i++) {
        if (board->squares[i][rank] == piece) {
            return i;
        }
    }

    return BOARD_FILE_OFF_BOARD;
}

board_piece_t board_get_piece(board_t *board, board_file_t file,
                              board_rank_t rank) {
    return board->squares[file][rank];
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
