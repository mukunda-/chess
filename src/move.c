#include "move.h"

movelist_t* movelist_new(void) {
    movelist_t* moves = (movelist_t*)malloc(sizeof(movelist_t));
    assert(moves != NULL && "Out of memory");
    memset(moves, 0, sizeof(movelist_t));

    return moves;
}

void move_free(move_t* move) {
    if (move == NULL) {
        return;
    }

    move_free(move->next);
    free(move);
}

move_t* move_new(square_t from, square_t to) {
    move_t* move = (move_t*)malloc(sizeof(move_t));
    assert(move != NULL && "Out of memory");
    memset(move, 0, sizeof(move_t));

    move->from = from;
    move->to = to;

    return move;
}

void movelist_free(movelist_t* moves) {
    move_free(moves->head);
    free(moves);
}

void movelist_insert_end(movelist_t* moves, move_t* move) {
    if (moves->head == NULL) {
        moves->head = move;
        moves->tail = move;
    }

    moves->tail->next = move;
    moves->tail = move;
}

size_t movelist_count(movelist_t* moves) {
    size_t count = 0;
    for (move_t* move = moves->head; move != NULL; move = move->next) {
        count++;
    }

    return count;
}

void movelist_remove_start(movelist_t* moves) {
    if (moves->head == NULL) {
        return;
    }

    if (moves->head == moves->tail) {
        move_free(moves->head);
        moves->head = NULL;
        moves->tail = NULL;

        return;
    }

    move_t* old_head = moves->head;
    move_t* head = old_head->next;

    old_head->next = NULL;
    move_free(old_head);

    if (head->next == NULL) {
        moves->tail = head;
    }

    moves->head = head;
}
