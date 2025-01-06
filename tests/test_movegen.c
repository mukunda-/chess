#include "board.h"
#include "move.h"
#include "movegen.h"
#include "square.h"
#include "test.h"

int main(void) {
    test_start("movegen.h");
    board_t *board = board_new();
    board->squares[SQUARE_E4] = SQUARE_ROOK_WHITE;

    movelist_t *moves = movegen_rook(board, SQUARE_E4);

    move_t *move = moves->head;
    assert_true(move);
    assert_true(move->from == SQUARE_E4);

    assert_true(movelist_count(moves) == 14);

    // Put a pawn in the way
    board->squares[SQUARE_E5] = SQUARE_ROOK_BLACK;

    movelist_free(moves);
    moves = movegen_rook(board, SQUARE_E4);

    assert_true(movelist_count(moves) == 11);

    movelist_free(moves);
    board_free(board);

    test_end();
}
