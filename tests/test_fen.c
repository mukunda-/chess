#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "fen.h"
#include "test.h"

int main(void) {
    test_start("fen.h");

    board_t* board = board_new(WHITE);
    assert_true(board);

    const char* fen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    fen_parse(fen, board);

    // Kings on their rightful squares
    assert_true(board_get_piece(board, SQUARE_E1) == SQUARE_KING_WHITE);
    assert_true(board_get_piece(board, SQUARE_E8) == SQUARE_KING_BLACK);

    // Convert back to FEN and compare
    char* built_fen = NULL;
    fen_build(&built_fen, board);
    assert_true(strcmp(fen, built_fen) == 0);
    free(built_fen);

    board_free(board);
    test_end();
}
