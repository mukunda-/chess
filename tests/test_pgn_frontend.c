#include "pgn_frontend.h"
#include "test.h"

int main(void) {
    test_start("pgn_frontend.h");

    pgn_frontend_t* env = pgn_frontend_new();
    assert_true(env);

    pgn_frontend_free(env);

    test_end();
}
