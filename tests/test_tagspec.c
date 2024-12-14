#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tagspec.h"

int main() {
    tagspec_t* spec = tagspec_new();
    assert(spec);

    tagspec_add(spec, "WhiteTitle", "BOT", TAG_NOT_EQUALS);
    assert(strcmp(spec->head->name, "WhiteTitle") == 0);

    tagspec_add(spec, "BlackTitle", "BOT", TAG_NOT_EQUALS);
    assert(spec->head->next != NULL);

    tagspec_free(spec);
}
