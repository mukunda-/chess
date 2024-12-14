#ifndef gameclock_H
#define gameclock_H

typedef struct timestamp_t {
    int time;
    struct timestamp_t* next;
} timestamp_t;

typedef struct gameclock_t {
    timestamp_t* head;
    timestamp_t* tail;
} gameclock_t;

gameclock_t* gameclock_new();
void gameclock_add(gameclock_t* gameclock, const char* h_m_s);

void gameclock_free(gameclock_t* gameclock);

#endif
