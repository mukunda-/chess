#ifndef TAGSPEC_H
#define TAGSPEC_H

#include <stdbool.h>

typedef enum {
    TAG_EQUALS = 0,
    TAG_NOT_EQUALS,
    TAG_CONTAINS,
    TAG_NOT_CONTAINS,
    TAG_NEVER,
    TAG_ALWAYS,
} tagcmp_kind_t;

typedef struct tagspec_t {
    struct tagcmp_t *head;
    struct tagcmp_t *tail;
} tagspec_t;

typedef struct tagcmp_t {
    tagcmp_kind_t kind;
    char *name;
    char *value;
    struct tagcmp_t *next;
} tagcmp_t;

/* Create a new tag */
tagspec_t *tagspec_new();

/* Dealocate tags and children */
void tagspec_free(tagspec_t *order);

/* Create a new tag */
tagspec_t *tagspec_new();

/* Dealocate tags and children */
void tagspec_free(tagspec_t *order);

/* Returns whether the given tag matches tagspec */
bool tagspec_matches(tagspec_t *order, const char *name, const char *value);

/* Add to the tag specification */
void tagspec_add(tagspec_t *tags, const char *name, const char *value,
                 tagcmp_kind_t kind);

#endif
