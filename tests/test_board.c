
#include "board.h"
#include "test.h"

int main(void) {
    test_start("board.h");

    board_t* board = board_new();
    assert_true(board);

    board_set_piece(board, BOARD_FILE_B, BOARD_RANK_1,
                    BOARD_PIECE_KNIGHT_WHITE);

    assert_true(board->squares[BOARD_FILE_B][BOARD_RANK_1] ==
                BOARD_PIECE_KNIGHT_WHITE);

    assert_true(board_get_piece(board, BOARD_FILE_B, BOARD_RANK_1) ==
                BOARD_PIECE_KNIGHT_WHITE);

    assert_true(board_get_rank(board, BOARD_PIECE_KNIGHT_WHITE, BOARD_FILE_B) ==
                BOARD_RANK_1);

    assert_true(board_get_file(board, BOARD_PIECE_KNIGHT_WHITE, BOARD_RANK_1) ==
                BOARD_FILE_B);

    board_free(board);

    test_end();
}
