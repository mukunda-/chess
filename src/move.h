#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include <stdlib.h>

#include "square.h"

typedef struct move {
    square_t from;
    square_t to;
    bool capture;
    struct move* next;
} move_t;

typedef struct movelist {
    move_t* head;
    move_t* tail;
} movelist_t;

movelist_t* movelist_new(void);

void movelist_free(movelist_t* moves);

void movelist_insert_end(movelist_t* moves, move_t* move);

move_t* move_new(square_t from, square_t to);

void move_free(move_t* move);

size_t movelist_count(movelist_t* moves);

void movelist_remove_start(movelist_t* moves);

#endif
