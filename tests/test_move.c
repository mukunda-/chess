#include <stdio.h>
#include <stdlib.h>

#include "move.h"
#include "test.h"

int main(void) {
    test_start("movelist_t");

    movelist_t* moves = movelist_new();
    assert_true(moves != NULL);

    movelist_add(moves, MOVE_TYPE_MOVE, "e4");
    assert_true(moves->head != NULL);

    movelist_free(moves);

    test_end();

    return EXIT_SUCCESS;
}
