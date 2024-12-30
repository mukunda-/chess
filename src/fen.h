#ifndef FEN_H
#define FEN_H

#include "board.h"

void fen_parse(const char *fen, board_t *board);
void fen_build(char **fen_out, board_t *board);

#endif
