#include "tagspec.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

tagcmp_t *tagcmp_new(const char *name, const char *value, tagcmp_kind_t kind) {
    tagcmp_t *tag = malloc(sizeof(tagcmp_t));

    tag->name = strdup(name);
    tag->value = strdup(value);
    tag->kind = kind;
    tag->next = NULL;

    return tag;
}

void tagspec_add(tagspec_t *tags, const char *name, const char *value,
                 tagcmp_kind_t kind) {
    tagcmp_t *cmp = tagcmp_new(name, value, kind);

    if (tags->head == NULL || tags->tail == NULL) {
        tags->head = cmp;
        tags->tail = cmp;
    } else {
        tags->tail->next = cmp;
        tags->tail = tags->tail->next;
    }

    if (tags->head == NULL) {
        tags->head = tags->tail;
    }
}

tagspec_t *tagspec_new() {
    tagspec_t *tags = malloc(sizeof(tagspec_t));

    tags->head = NULL;
    tags->tail = NULL;

    return tags;
}

void tagcmp_free(tagcmp_t *cmp) {
    if (cmp == NULL) {
        return;
    }

    free(cmp->name);
    free(cmp->value);

    tagcmp_free(cmp->next);

    free(cmp);
}

void tagspec_free(tagspec_t *tags) {
    if (tags == NULL) {
        return;
    }

    tagcmp_free(tags->head);

    free(tags);
}
