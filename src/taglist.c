#include "taglist.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "tagspec.h"

tag_t *tag_new(const char *name, const char *value, tag_t *next) {
    tag_t *tag = malloc(sizeof(tag_t));

    tag->name = strdup(name);
    tag->value = strdup(value);
    tag->next = next;

    return tag;
}

void taglist_add(taglist_t *tags, const char *name, const char *value) {
    tag_t *tag = tag_new(name, value, NULL);
    if (tags->tail == NULL) {
        tags->tail = tag;
        tags->head = tag;
    } else {
        tags->tail->next = tag;
        tags->tail = tags->tail->next;
    }
}

taglist_t *taglist_new() {
    taglist_t *tags = malloc(sizeof(taglist_t));

    tags->head = NULL;
    tags->tail = NULL;

    return tags;
}

void tag_free(tag_t *tag) {
    if (tag == NULL) {
        return;
    }

    free(tag->name);
    free(tag->value);

    tag_free(tag->next);

    free(tag);
}

void taglist_free(taglist_t *tags) {
    if (tags == NULL) {
        return;
    }

    tag_free(tags->head);

    free(tags);
}

taglist_t *taglist_new_aligned(taglist_t *tags, tagspec_t *spec) {
    taglist_t *aligned_tags = taglist_new();

    for (tagcmp_t *cmp = spec->head; cmp != NULL; cmp = cmp->next) {
        // Find the tag corresponding to the comparior
        bool found = false;
        for (tag_t *tag = tags->head; tag != NULL; tag = tag->next) {
            if (strcmp(cmp->name, tag->name) == 0) {
                taglist_add(aligned_tags, tag->name, tag->value);
                found = true;
                break;
            }
        }

        if (!found) {
            taglist_add(aligned_tags, cmp->name, "");
        }
    }

    return aligned_tags;
}
