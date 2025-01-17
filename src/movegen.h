#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "global.h"
#include "move.h"
#include "square.h"

bool movegen(movelist_t* moves_out, board_t* board);

void movegen_rook(movelist_t* moves_out, board_t* board, square_t from);

void movegen_bishop(movelist_t* moves_out, board_t* board, square_t from);

void movegen_queen(movelist_t* moves_out, board_t* board, square_t from);

void movegen_pawn(movelist_t* moves_out, board_t* board, square_t from);

void movegen_king(movelist_t* moves_out, board_t* board, square_t from);

void movegen_knight(movelist_t* moves_out, board_t* board, square_t from);

movelist_t* movegen_subgraph(movelist_t* moves, square_t center);

#endif /* MOVEGEN_H */
