#include "move.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

movelist_t *movelist_new(void) {
    movelist_t *moves = malloc(sizeof(struct movelist_t));

    moves->head = NULL;
    moves->tail = NULL;

    return moves;
}

void movelist_add(movelist_t *moves, move_type_t kind, const char *value) {
    struct move_t *move = malloc(sizeof(struct move_t));

    move->next = NULL;
    move->kind = kind;
    move->value = strdup(value);

    if (moves->head == NULL) {
        moves->head = move;
        moves->tail = move;
    } else {
        moves->tail->next = move;
        moves->tail = moves->tail->next;
    }
}

void movelist_free(movelist_t *moves) {
    if (moves == NULL) {
        return;
    }

    struct move_t *head = moves->head;

    while (head != NULL) {
        struct move_t *next = head->next;

        free(head->value);
        free(head);

        head = next;
    }

    free(moves);
}
