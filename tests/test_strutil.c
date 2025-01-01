#include <stdlib.h>
#include <string.h>

#include "strutil.h"
#include "test.h"

int main(void) {
    test_start("strutil.h");

    char *msg = strdup("foo   ");

    trim_right(msg);

    assert_true(strcmp(msg, "foo") == 0);

    test_end();

    free(msg);

    return EXIT_SUCCESS;
}
