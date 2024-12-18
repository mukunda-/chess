#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tag roster>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file in read mode, exit with error if failure to do so
    const char *roster_path = argv[1];
    FILE *roster_fp = fopen(roster_path, "r");
    if (roster_fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    frontend_t *env = frontend_new();

    tagspec_load(env->spec, roster_fp);
    fclose(roster_fp);

    run(env);

    frontend_free(env);

    return EXIT_SUCCESS;
}
