#include <stdlib.h>

#include "tagspec.h"
#include "test.h"

int main(void) {
    test_start("tagspec_t");

    tagspec_t* spec = tagspec_new();
    assert_true(spec);

    tagspec_free(spec);

    test_end();

    return EXIT_SUCCESS;
}
