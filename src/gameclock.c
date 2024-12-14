#include "gameclock.h"

#include <stdlib.h>
#include <strings.h>

gameclock_t* gameclock_new() {
    gameclock_t* gameclock = malloc(sizeof(gameclock_t));

    gameclock->head = NULL;
    gameclock->tail = NULL;

    return gameclock;
}

void gameclock_add(gameclock_t* gameclock, const char* h_m_s) {
    timestamp_t* next = malloc(sizeof(timestamp_t));
    next->time = 0;
    next->next = NULL;

    // Foreach chunk
    const char* head = h_m_s;
    for (int i = 0; i < 3; i++) {
        int chunk = atoi(head);
        int multiplier = (2 - i) * 60;
        if (multiplier != 0) {
            chunk *= multiplier;
        }

        next->time += chunk;

        head = index(head, ':');
        if (head == NULL) {
            break;
        }

        head++;
    }

    if (gameclock->tail == NULL) {
        gameclock->head = next;
        gameclock->tail = next;
    } else {
        gameclock->tail->next = next;
        gameclock->tail = gameclock->tail->next;
    }
}

void timestamp_free(timestamp_t* time) {
    if (time == NULL) {
        return;
    }

    timestamp_free(time->next);

    free(time);
}

void gameclock_free(gameclock_t* gameclock) {
    timestamp_free(gameclock->head);
    free(gameclock);
}
