#ifndef TAGSPEC_H
#define TAGSPEC_H

#include <stdbool.h>
#include <stdio.h>

typedef enum tagcmp_kind {
    TAG_EQUALS = 0,
    TAG_NOT_EQUALS,
    TAG_CONTAINS,
    TAG_GREATER_THAN,
    TAG_NOT_CONTAINS,
    TAG_ALWAYS,
    TAG_UNKNOWN_CMP,
} tagcmp_kind_t;

typedef enum tagop_kind {
    TAG_OP_SPLIT = 0,
    TAG_OP_UNKNOWN,
} tagop_kind_t;

typedef struct tagorder {
    char *name;
    struct tagorder *next;
} tagorder_t;

typedef struct tagcmp {
    tagcmp_kind_t kind;
    char *name;
    char *value;
    struct tagcmp *next;
} tagcmp_t;

typedef struct tagspec {
    tagcmp_t *head;
    tagcmp_t *tail;
    tagorder_t *order_head;
    tagorder_t *order_tail;
} tagspec_t;

/* Create a new tag */
tagspec_t *tagspec_new(void);

/* Dealocate tags and children */
void tagspec_free(tagspec_t *spec);

/* Returns whether the given tag matches tagspec */
bool tagspec_matches(tagspec_t *spec, const char *name, const char *value);

/* Add to the tag specification */
void tagspec_add(tagspec_t *spec, const char *name, const char *value,
                 tagcmp_kind_t kind);

/* Load tagspec from file on top of current spec */
void tagspec_load(tagspec_t *spec, FILE *spec_fp);

/* Parse tagspec line */
bool tagspec_parse_line(tagspec_t *spec, const char *line);

#endif
