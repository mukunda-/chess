#include "tagspec.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "strutil.h"

tagcmp_t *tagcmp_new(const char *name, const char *value, tagcmp_kind_t kind) {
    tagcmp_t* cmp = (tagcmp_t*)malloc(sizeof(tagcmp_t));
    assert(cmp != NULL && "Out of memory");
    memset(cmp, 0, sizeof(tagcmp_t));

    cmp->name = strdup(name);
    cmp->value = strdup(value);
    cmp->kind = kind;
    cmp->next = NULL;

    return cmp;
}

tagorder_t *tagorder_new(const char *name) {
    tagorder_t* tag = (tagorder_t*)malloc(sizeof(tagorder_t));
    assert(tag != NULL && "Out of memory");
    memset(tag, 0, sizeof(tagorder_t));

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
    tagspec_t* tags = (tagspec_t*)malloc(sizeof(tagspec_t));
    assert(tags != NULL && "Out of memory");
    memset(tags, 0, sizeof(tagspec_t));

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
    tagcmp_kind_t result = TAG_UNKNOWN_CMP;
    switch (opperator) {
        case '!':
            result = TAG_NOT_EQUALS;
            break;
        case '=':
            result = TAG_EQUALS;
            break;
        case '-':
            result = TAG_NOT_CONTAINS;
            break;
        case '~':
            result = TAG_CONTAINS;
            break;
        case '>':
            result = TAG_GREATER_THAN;
            break;
        default:
            break;
    }

    return result;
}

bool tagspec_parse_line(tagspec_t *spec, const char *line) {

    // We must use sizeof(char) since we cannot guarantee it always equals 1 in multyverse.
    char* name = (char*)calloc(BUFF_SIZE_256, sizeof(char));
    char* value = (char*)calloc(BUFF_SIZE_256, sizeof(char));
    char* cmp = (char*)calloc(BUFF_SIZE_32, sizeof(char));

    assert(name != NULL && "Out of memory");
    assert(value != NULL && "Out of memory");
    assert(cmp != NULL && "Out of memory");

    sscanf(line, "%[^\t\n ] %[^\t\n ] %[^\n]", name, cmp, value);

    bool success = false;
    if (name[0] != '\0' && value[0] != '\0') {
        tagcmp_kind_t kind = tagspec_get_kind(cmp[0]);
        if (kind != TAG_UNKNOWN_CMP) {
            tagspec_add(spec, name, value, kind);
            success = true;
        }
    } else if (name[0] != '\0') {
        tagspec_add(spec, name, NULL, TAG_ALWAYS);
        success = true;
    }

    free(name);
    free(value);
    free(cmp);

    return success;
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
    bool result = true;
    for (tagcmp_t *cmp = spec->head; cmp != NULL; cmp = cmp->next) {
        if (strcmp(name, cmp->name) != 0) {
            continue;
        }

        switch (cmp->kind) {
            case TAG_EQUALS:
                result = strcmp(value, cmp->value) == 0;
                break;
            case TAG_NOT_EQUALS:
                result = strcmp(value, cmp->value) != 0;
                break;
            case TAG_CONTAINS:
                result = strstr(value, cmp->value) != NULL;
                break;
            case TAG_NOT_CONTAINS:
                result = strstr(value, cmp->value) == NULL;
                break;
            case TAG_GREATER_THAN:
                result = atoi(value) > atoi(cmp->value);
                break;
            case TAG_ALWAYS:
                result = true;
                break;
            default:
                fprintf(stderr, "Unknown operator: %c", cmp->kind);
                assert(false && "unimplemented");
        }
    }

    return result;
}
