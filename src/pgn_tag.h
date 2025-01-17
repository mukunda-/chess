#ifndef TAGLIST_H
#define TAGLIST_H

#include "tagspec.h"

typedef struct taglist_t {
    struct tag_t *head;
    struct tag_t *tail;
} taglist_t;

typedef struct tag_t {
    char *name;
    char *value;
    struct tag_t *next;
} tag_t;

/* Add a tag to the taglist.
 * Takes ownership of its memory.
 */
void taglist_add(taglist_t *tags, const char *name, const char *value);

/* Create a new tag */
taglist_t *taglist_new(void);

/* Dealocate tags and children */
void taglist_free(taglist_t *tags);

/* Prune and expand taglist to match tagspec */
taglist_t *taglist_new_aligned(taglist_t *tags, tagspec_t *spec);

#endif
