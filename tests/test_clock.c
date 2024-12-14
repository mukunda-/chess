#include <assert.h>
#include <stdio.h>

#include "gameclock.h"

int main() {
    gameclock_t* clock = gameclock_new();
    assert(clock);

    gameclock_add(clock, "0:05:00");
    assert(clock->tail->time == 300);

    gameclock_add(clock, "0:04:00");
    assert(clock->tail->time == 240);

    gameclock_add(clock, "0:03:43");
    assert(clock->tail->time == 223);

    gameclock_free(clock);
}
