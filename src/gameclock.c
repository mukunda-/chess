#include "gameclock.h"

#include <stdlib.h>
#include <strings.h>

gameclock_t* gameclock_new(void) {
    gameclock_t* gameclock = (gameclock_t*)malloc(sizeof(gameclock_t));
    assert(gameclock != NULL && "Out of memory");
    memset(gameclock, 0, sizeof(gameclock_t));

    return gameclock;
}

int parse_timestamp(const char* h_m_s) {
    int seconds = 0;

    // Foreach chunk
    const char* head = h_m_s;
    for (int i = 0; i < 3; i++) {
        int chunk = atoi(head);
        int multiplier = (2 - i) * GAMECLOCK_MINUTE;
        if (multiplier != 0) {
            chunk *= multiplier;
        }

        seconds += chunk;

        head = index(head, ':');
        if (head == NULL) {
            break;
        }

        head++;
    }

    return seconds;
}

void gameclock_add(gameclock_t* gameclock, const char* h_m_s) {
    timestamp_t* next = (timestamp_t*)malloc(sizeof(timestamp_t));
    assert(next != NULL && "Out of memory");
    memset(next, 0, sizeof(timestamp_t));

    next->time = parse_timestamp(h_m_s);

    if (gameclock->tail == NULL) {
        gameclock->head = next;
        gameclock->tail = next;
    } else {
        gameclock->tail->next = next;
        gameclock->tail = gameclock->tail->next;
    }
}

void timestamp_free(timestamp_t* time) {
    while (time != NULL) {
        timestamp_t* next = time->next;

        free(time);

        time = next;
    }
}

void gameclock_free(gameclock_t* gameclock) {
    timestamp_free(gameclock->head);
    free(gameclock);
}
