#include <assert.h>
#include <stdio.h>

#include "tagspec.h"

int main() {
    tagspec_t* spec = tagspec_new();

    tagspec_add(spec, "WhiteTitle", "BOT", TAG_NOT_EQUALS);
    tagspec_add(spec, "BlackTitle", "BOT", TAG_NOT_EQUALS);

    tagspec_free(spec);
    printf("ok\n");
}
