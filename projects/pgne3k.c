#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tag spec>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file in read mode, exit with error if failure to do so
    const char *spec_path = argv[1];
    FILE *spec_fp = fopen(spec_path, "r");
    if (spec_fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    frontend_t *env = frontend_new();

    tagspec_load(env->spec, spec_fp);
    fclose(spec_fp);

    run(env);

    frontend_free(env);

    return EXIT_SUCCESS;
}
