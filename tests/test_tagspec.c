#include <stdio.h>
#include <string.h>

#include "tagspec.h"
#include "test.h"

void test_match(void) {
    tagspec_t* spec = tagspec_new();
    tagspec_parse_line(spec, "Site");
    tagspec_parse_line(spec, "WhiteElo");
    tagspec_parse_line(spec, "BlackElo");
    tagspec_parse_line(spec, "Ply > 3");
    tagspec_parse_line(spec, "WhiteClock");
    tagspec_parse_line(spec, "BlackClock");
    tagspec_parse_line(spec, "WhiteTitle ! BOT");
    tagspec_parse_line(spec, "BlackTitle ! BOT");
    tagspec_parse_line(spec, "Result ! *");
    tagspec_parse_line(spec, "Event = Rated Classical game");

    assert_true(tagspec_matches(spec, "Ply", "4"));
    assert_true(!tagspec_matches(spec, "Ply", "3"));

    assert_true(tagspec_matches(spec, "WhiteTitle", "HUMAN"));
    assert_true(!tagspec_matches(spec, "WhiteTitle", "BOT"));

    assert_true(tagspec_matches(spec, "Event", "Rated Classical game"));
    assert_true(!tagspec_matches(spec, "Event", "Rated Bullet game"));

    tagspec_free(spec);
}

void test_parse(void) {
    tagspec_t* spec = tagspec_new();
    assert_true(spec);

    assert_true(tagspec_parse_line(spec, "Result ! *"));
    assert_true(spec->tail->kind == TAG_NOT_EQUALS);
    assert_true(strcmp(spec->tail->value, "*") == 0);
    assert_true(strcmp(spec->order_tail->name, "Result") == 0);
    assert_true(spec->order_head == spec->order_tail);

    assert_true(tagspec_parse_line(spec, "Event = Rated Classical game"));
    assert_true(spec->tail->kind == TAG_EQUALS);
    assert_true(strcmp(spec->tail->value, "Rated Classical game") == 0);

    assert_true(tagspec_parse_line(spec, "Site      "));
    assert_true(strcmp(spec->order_tail->name, "Site") == 0);
    assert_true(strcmp(spec->tail->name, "Site") != 0);

    assert_true(tagspec_parse_line(spec, "Ply > 2"));
    assert_true(spec->tail->kind == TAG_GREATER_THAN);
    assert_true(strcmp(spec->tail->value, "2") == 0);

    tagspec_free(spec);
}

int main(void) {
    test_start("tagspec.h");

    test_parse();
    test_match();

    test_end();
}
