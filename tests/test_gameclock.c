#include <stdio.h>
#include <stdlib.h>

#include "gameclock.h"

// Test helpers
#include "test.h"

int main(void) {
    test_start("gameclock_t");
    gameclock_t* clock = gameclock_new();
    assert_true(clock != NULL);

    gameclock_add(clock, "0:05:00");
    assert_true(clock->tail->time == 300);

    gameclock_add(clock, "0:04:00");
    assert_true(clock->tail->time == 240);

    gameclock_add(clock, "0:03:43");
    assert_true(clock->tail->time == 223);

    gameclock_free(clock);

    test_end();

    return EXIT_SUCCESS;
}
