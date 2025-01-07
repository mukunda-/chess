#include "board.h"
#include "test.h"

int main(void) {
    test_start("board.h");

    board_t* board = board_new(WHITE);
    assert_true(board);

    board_set_piece(board, SQUARE_B1, SQUARE_KNIGHT_WHITE);

    assert_true(board_get_piece(board, SQUARE_B1) == SQUARE_KNIGHT_WHITE);

    assert_true(board_get_rank(board, SQUARE_KNIGHT_WHITE, SQUARE_FILE_B) ==
                SQUARE_RANK_1);

    assert_true(board_get_file(board, SQUARE_KNIGHT_WHITE, SQUARE_RANK_1) ==
                SQUARE_FILE_B);

    board_free(board);

    test_end();

    return 0;
}
