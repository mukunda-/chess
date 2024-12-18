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

void tagspec_add(tagspec_t *spec, const char *name, const char *value,
                 tagcmp_kind_t kind) {
    tagcmp_t *cmp = tagcmp_new(name, value, kind);

    if (spec->head == NULL || spec->tail == NULL) {
        spec->head = cmp;
        spec->tail = cmp;
    } else {
        spec->tail->next = cmp;
        spec->tail = spec->tail->next;
    }

    if (spec->head == NULL) {
        spec->head = spec->tail;
    }
}

tagspec_t *tagspec_new(void) {
    tagspec_t *tags = malloc(sizeof(tagspec_t));

    tags->head = NULL;
    tags->tail = NULL;

    return tags;
}

void tagcmp_free(tagcmp_t *cmp) {
    while (cmp != NULL) {
        tagcmp_t *next = cmp->next;

        free(cmp->name);
        free(cmp->value);
        free(cmp);

        cmp = next;
    }
}

void tagspec_free(tagspec_t *spec) {
    if (spec == NULL) {
        return;
    }

    tagcmp_free(spec->head);

    free(spec);
}

tagcmp_kind_t tagspec_get_kind(char opperator) {
    switch (opperator) {
        case '!':
            return TAG_NOT_EQUALS;
        case '=':
            return TAG_EQUALS;
        case '-':
            return TAG_NOT_CONTAINS;
        case '~':
            return TAG_CONTAINS;
    }

    return TAG_ALWAYS;
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

    tagcmp_kind_t kind = tagspec_get_kind(*at_op);
    tagspec_add(spec, buf, at_value, kind);

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
