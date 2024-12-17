#include <stdio.h>
#include <string.h>

#include "game.h"
#include "test.h"

int main(void) {
    test_start("gamelist_t");

    gamelist_t* games = gamelist_new();
    assert_true(games != NULL);

    gamelist_add_result(games, "*");
    assert_true(games->head != NULL);

    game_t* game = games->head;

    assert_true(game->ply == 0);
    assert_true(strcmp(game->result, "*") == 0);

    gamelist_free(games);

    test_end();
}
