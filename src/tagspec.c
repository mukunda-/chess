#include "tagspec.h"

#include <assert.h>
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

tagorder_t *tagorder_new(const char *name) {
    tagorder_t *tag = malloc(sizeof(tagorder_t));

    tag->name = strdup(name);
    tag->next = NULL;

    return tag;
}

void tagorder_free(tagorder_t *tag) {
    if (tag == NULL) {
        return;
    }

    tagorder_free(tag->next);
    free(tag->name);
    free(tag);
}

void tagspec_add(tagspec_t *spec, const char *name, const char *value,
                 tagcmp_kind_t kind) {
    if (kind != TAG_ALWAYS) {
        tagcmp_t *cmp = tagcmp_new(name, value, kind);

        if (spec->head == NULL) {
            spec->head = cmp;
            spec->tail = cmp;
        } else {
            spec->tail->next = cmp;
            spec->tail = spec->tail->next;
        }
    }

    tagorder_t *tag = tagorder_new(name);

    if (spec->order_head == NULL) {
        spec->order_head = tag;
        spec->order_tail = tag;
    } else {
        spec->order_tail->next = tag;
        spec->order_tail = spec->order_tail->next;
    }
}

tagspec_t *tagspec_new(void) {
    tagspec_t *tags = malloc(sizeof(tagspec_t));

    tags->head = NULL;
    tags->tail = NULL;

    tags->order_head = NULL;
    tags->order_tail = NULL;

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
    tagorder_free(spec->order_head);

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
        case '>':
            return TAG_GREATER_THAN;
    }

    return TAG_UNKNOWN_OP;
}

bool tagspec_parse_line(tagspec_t *spec, const char *line) {
    char *buf = strdup(line);
    trim_right(buf);

    char *saveptr = NULL;
    const char *delims = " ";
    const char *name = strtok_r(buf, delims, &saveptr);

    const char *op_raw = strtok_r(NULL, delims, &saveptr);
    if (op_raw == NULL) {
        tagspec_add(spec, name, NULL, TAG_ALWAYS);
        free(buf);
        return true;
    }

    tagcmp_kind_t kind = tagspec_get_kind(op_raw[0]);
    if (kind == TAG_UNKNOWN_OP) {
        free(buf);
        return false;
    }

    const char *value = strtok_r(NULL, delims, &saveptr);
    if (value == NULL) {
        free(buf);
        return false;
    }

    tagspec_add(spec, name, value, kind);
    free(buf);

    return true;
}

void tagspec_load(tagspec_t *spec, FILE *spec_fp) {
    char *line = NULL;
    size_t line_len;
    while (getline(&line, &line_len, spec_fp) != -1) {
        assert(tagspec_parse_line(spec, line));
    }

    free(line);
}

bool tagspec_matches(tagspec_t *spec, const char *name, const char *value) {
    for (tagcmp_t *cmp = spec->head; cmp != NULL; cmp = cmp->next) {
        if (strcmp(name, cmp->name) != 0) {
            continue;
        }

        switch (cmp->kind) {
            case TAG_EQUALS:
                return strcmp(value, cmp->value) == 0;
            case TAG_NOT_EQUALS:
                return strcmp(value, cmp->value) != 0;
            case TAG_CONTAINS:
                return strstr(value, cmp->value) != NULL;
            case TAG_NOT_CONTAINS:
                return strstr(value, cmp->value) == NULL;
            case TAG_GREATER_THAN:
                return atoi(value) > atoi(cmp->value);
            case TAG_ALWAYS:
                return true;
            default:
                fprintf(stderr, "Unknown operator: %c", cmp->kind);
                assert(false && "unimplemented");
        }
    }

    return true;
}
