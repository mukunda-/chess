#include <stdio.h>
#include <string.h>

#include "pgn.h"
#include "test.h"

int main(void) {
    test_start("pgn.h");

    pgnlist_t* pgns = pgnlist_new();
    assert_true(pgns != NULL);

    pgnlist_add_result(pgns, "*");
    assert_true(pgns->head != NULL);

    pgn_t* pgn = pgns->head;

    assert_true(pgn->ply == 0);
    assert_true(strcmp(pgn->result, "*") == 0);

    pgnlist_free(pgns);

    test_end();
}
