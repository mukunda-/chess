#include <assert.h>

#define assert_true(x) \
    assert(x);         \
    printf(".");

#define test_start(x) \
    printf(x);        \
    printf(" ");

#define test_end() printf(" ok\n");
