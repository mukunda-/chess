#ifndef gameclock_H
#define gameclock_H

#define GAMECLOCK_MINUTE 60

typedef struct timestamp {
    int time;
    struct timestamp* next;
} timestamp_t;

typedef struct gameclock {
    timestamp_t* head;
    timestamp_t* tail;
} gameclock_t;

gameclock_t* gameclock_new(void);
void gameclock_add(gameclock_t* gameclock, const char* h_m_s);

void gameclock_free(gameclock_t* gameclock);

#endif
