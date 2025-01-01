#include "pgn_tag.h"
#include "test.h"

int main(void) {
    test_start("taglist_t");

    taglist_t* tags = taglist_new();
    assert_true(tags);

    taglist_add(tags, "Event", "Fun Fest");
    assert_true(tags->head != NULL);

    taglist_add(tags, "White", "Murphy");
    assert_true(tags->head->next != NULL);

    taglist_free(tags);

    test_end();

    return 0;
}
