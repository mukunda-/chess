#include "tagspec.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "strutil.h"

tagcmp_t *tagcmp_new(const char *name, const char *value, tagcmp_kind_t kind) {
    tagcmp_t *tag = malloc(sizeof(tagcmp_t));

    tag->name = strdup(name);
    if (value == NULL) {
        tag->value = NULL;
    } else {
        tag->value = strdup(value);
    }
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

tagspec_t *tagspec_new(void) {
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

void tagspec_add_line(tagspec_t *spec, const char *line) {
    char *buf = strdup(line);
    trim_right(buf);

    // Operator
    char *at_op = index(buf, ' ');
    if (at_op == NULL) {
        tagspec_add(spec, buf, NULL, TAG_ALWAYS);
        free(buf);
        return;
    }
    at_op[0] = '\0';
    at_op++;

    char *at_value = index(at_op, ' ');
    if (at_value != NULL) {
        at_value[0] = '\0';
        at_value++;
    } else {
        at_value = "";
    }

    switch (*at_op) {
        case '!':
            tagspec_add(spec, buf, at_value, TAG_NOT_EQUALS);
            break;
        case '=':
            tagspec_add(spec, buf, at_value, TAG_EQUALS);
            break;
        case '-':
            tagspec_add(spec, buf, at_value, TAG_NOT_CONTAINS);
            break;
        case '~':
            tagspec_add(spec, buf, at_value, TAG_CONTAINS);
            break;
        default:
            tagspec_add(spec, buf, NULL, TAG_ALWAYS);
    }

    free(buf);
}

void tagspec_load(tagspec_t *spec, FILE *roster_fp) {
    char *line = NULL;
    size_t line_len;
    while (getline(&line, &line_len, roster_fp) != -1) {
        tagspec_add_line(spec, line);
    }

    free(line);
}
