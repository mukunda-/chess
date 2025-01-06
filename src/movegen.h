#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "board.h"
#include "move.h"
#include "square.h"

bool movegen(movelist_t* moves_out, board_t* board, board_turn_t turn);

void movegen_rook(movelist_t* moves_out, board_t* board, square_t from);

void movegen_bishop(movelist_t* moves_out, board_t* board, square_t from);

void movegen_queen(movelist_t* moves_out, board_t* board, square_t from);

void movegen_pawn(movelist_t* moves_out, board_t* board, square_t from);

void movegen_king(movelist_t* moves_out, board_t* board, square_t from);

void movegen_knight(movelist_t* moves_out, board_t* board, square_t from);

#endif
