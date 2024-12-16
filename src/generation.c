#include "generation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gameclock.h"
#include "gamelist.h"
#include "movelist.h"
#include "taglist.h"
#include "tagspec.h"

void print_headers(tagspec_t *spec) {
    for (tagcmp_t *cmp = spec->head; cmp != NULL; cmp = cmp->next) {
        printf("%s\t", cmp->name);
    }

    printf("\n");
}

void print_moves(move_t *moves_head) {
    int ply = 0;
    for (move_t *move = moves_head; move != NULL; move = move->next) {
        if (move->kind == MOVE_TYPE_MOVE) {
            if (ply++ > 0) {
                printf(" ");
            }

            printf("%s", move->value);
        }
    }
}

void print_clock(gameclock_t *clock) {
    for (timestamp_t *ts = clock->head; ts != NULL; ts = ts->next) {
        if (ts != clock->head) {
            printf(" ");
        }

        printf("%d", ts->time);
    }
}

void print_game(tagspec_t *spec, game_t *game) {
    for (tagcmp_t *cmp = spec->head; cmp != NULL; cmp = cmp->next) {
        for (tag_t *tag = game->tags->head; tag != NULL; tag = tag->next) {
            if (strcmp(tag->name, cmp->name) != 0) {
                continue;
            }

            if (cmp->kind == TAG_EQUALS &&
                (strcmp(tag->value, cmp->value) != 0)) {
                return;
            }

            if (cmp->kind == TAG_NOT_EQUALS &&
                (strcmp(tag->value, cmp->value) == 0)) {
                return;
            }

            if (cmp->kind == TAG_CONTAINS &&
                (strstr(tag->value, cmp->value) == NULL)) {
                return;
            }

            if (cmp->kind == TAG_NOT_CONTAINS &&
                (strstr(tag->value, cmp->value) != NULL)) {
                return;
            }
        }
    }

    taglist_t *aligned_tags = taglist_new_aligned(game->tags, spec);
    for (tag_t *tag = aligned_tags->head; tag != NULL; tag = tag->next) {
        if (aligned_tags->head != tag) {
            printf("\t");
        }

        if (strcmp(tag->name, "Moves") == 0) {
            print_moves(game->moves->head);
        } else if (strcmp(tag->name, "Ply") == 0) {
            printf("%d", game->ply);
        } else if (strcmp(tag->name, "WhiteClock") == 0) {
            print_clock(game->clock_white);
        } else if (strcmp(tag->name, "BlackClock") == 0) {
            print_clock(game->clock_black);
        } else if (tag->value == NULL && strcmp(tag->name, "Result") == 0) {
            printf("%s", game->result);
        } else if (tag->value != NULL) {
            printf("%s", tag->value);
        }
    }

    printf("\n");

    taglist_free(aligned_tags);
}
