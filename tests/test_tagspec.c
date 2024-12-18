#include <stdio.h>
#include <string.h>

#include "tagspec.h"
#include "test.h"

int main(void) {
    test_start("tagspec_t");

    tagspec_t* spec = tagspec_new();
    assert_true(spec);

    tagspec_add(spec, "WhiteTitle", "BOT", TAG_NOT_EQUALS);
    assert_true(strcmp(spec->head->name, "WhiteTitle") == 0);
    assert_true(spec->head->kind == TAG_NOT_EQUALS);
    assert_true(strcmp(spec->head->value, "BOT") == 0);

    assert_true(tagspec_matches(spec, "WhiteTitle", "Real hooman"));
    assert_true(tagspec_matches(spec, "unrelated tag", "blorp"));
    assert_true(tagspec_matches(spec, "unrelated tag", "BOT"));
    assert_true(!tagspec_matches(spec, "WhiteTitle", "BOT"));

    tagspec_add(spec, "BlackTitle", "BOT", TAG_NOT_EQUALS);
    assert_true(spec->head->next != NULL);
    assert_true(spec->head != spec->tail);

    assert_true(tagspec_parse_line(spec, "Result ! *"));
    assert_true(spec->tail->kind == TAG_NOT_EQUALS);
    assert_true(strcmp(spec->tail->value, "*") == 0);

    assert_true(tagspec_parse_line(spec, "Site      "));
    assert_true(spec->tail->kind == TAG_ALWAYS);
    assert_true(strcmp(spec->tail->name, "Site") == 0);
    assert_true(spec->tail->value == NULL);

    assert_true(tagspec_parse_line(spec, "Ply > 2"));
    assert_true(spec->tail->kind == TAG_GREATER_THAN);
    assert_true(strcmp(spec->tail->value, "2") == 0);
    assert_true(tagspec_matches(spec, "Ply", "3"));
    assert_true(!tagspec_matches(spec, "Ply", "2"));
    assert_true(!tagspec_matches(spec, "Ply", "1"));

    tagspec_free(spec);

    test_end();
}
