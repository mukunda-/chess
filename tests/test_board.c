
#include "board.h"
#include "test.h"

int main(void) {
    test_start("board_t");

    board_t* board = board_new();
    assert_true(board);

    board_set_piece(board, BOARD_FILE_E, BOARD_RANK_2, BOARD_PIECE_PAWN_WHITE);

    assert_true(board_get_piece(board, BOARD_FILE_E, BOARD_RANK_2) ==
                BOARD_PIECE_PAWN_WHITE);

    assert_true(board_get_rank(board, BOARD_PIECE_PAWN_WHITE, BOARD_FILE_E) ==
                BOARD_RANK_2);
    assert_true(board_get_file(board, BOARD_PIECE_PAWN_WHITE, BOARD_RANK_2) ==
                BOARD_FILE_E);

    test_end();
}
