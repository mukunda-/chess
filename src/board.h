#ifndef BOARD_H
#define BOARD_H

#include "square.h"

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

typedef enum board_turn {
    BLACK = 0,
    WHITE = 1,
} board_turn_t;

typedef struct board {
    squares_t squares;
    board_turn_t turn;
} board_t;

board_t *board_new(board_turn_t turn);

void board_free(board_t *board);

void board_print(board_t *board);

void board_set_piece(board_t *board, square_t square, square_piece_t piece);

square_piece_t board_get_piece(board_t *board, square_t square);

square_rank_t board_get_rank(board_t *board, square_piece_t piece,
                             square_file_t file);
square_file_t board_get_file(board_t *board, square_piece_t piece,
                             square_rank_t rank);

#endif
