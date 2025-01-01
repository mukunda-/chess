#include <stdlib.h>

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

    int count = 0;
    while (move != NULL) {
        count++;
        move = move->next;
    }

    printf("\n\n%d\n\n", count);
    assert_true(count == 14);

    free(board);

    test_end();
}
