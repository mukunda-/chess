#include "frontend.h"
#include "test.h"

int main(void) {
    test_start("frontend_t");

    frontend_t* env = frontend_new();
    assert_true(env);

    frontend_free(env);

    test_end();
}
