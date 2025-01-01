#include <stdio.h>
#include <stdlib.h>

#include "pgn_move.h"
#include "test.h"

int main(void) {
    test_start("pgn_move.h");

    pgn_movelist_t* moves = pgn_movelist_new();
    assert_true(moves != NULL);

    pgn_movelist_add(moves, PGN_MOVETYPE_MOVE, "e4");
    assert_true(moves->head != NULL);

    pgn_movelist_free(moves);

    test_end();

    return EXIT_SUCCESS;
}
