#include <stdio.h>
#include <string.h>

#include "tagspec.h"
#include "test.h"

int main() {
    test_start("tagspec_t");

    tagspec_t* spec = tagspec_new();
    assert_true(spec);

    tagspec_add(spec, "WhiteTitle", "BOT", TAG_NOT_EQUALS);
    assert_true(strcmp(spec->head->name, "WhiteTitle") == 0);
    assert_true(spec->head->kind == TAG_NOT_EQUALS);
    assert_true(strcmp(spec->head->value, "BOT") == 0);

    tagspec_add(spec, "BlackTitle", "BOT", TAG_NOT_EQUALS);
    assert_true(spec->head->next != NULL);
    assert_true(spec->head != spec->tail);

    tagspec_add_line(spec, "Result ! *");
    assert_true(spec->tail->kind == TAG_NOT_EQUALS);
    assert_true(strcmp(spec->tail->value, "*") == 0);

    tagspec_add_line(spec, "Site");
    assert_true(spec->tail->kind == TAG_ALWAYS);
    assert_true(spec->tail->value == NULL);

    tagspec_free(spec);

    test_end();
}
