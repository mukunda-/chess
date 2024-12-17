#include <stdlib.h>

#include "symbol.h"
#include "test.h"

int main(void) {
    test_start("symboltable_t");

    symboltable_t* spec = symboltable_new();
    assert_true(spec);

    symboltable_free(spec);

    test_end();

    return EXIT_SUCCESS;
}
