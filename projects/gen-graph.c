#include <board.h>
#include <fen.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv) {
    if (argc != 2) {
        printf("Usage: %s <FEN>\n", argv[0]);
    }

    return EXIT_SUCCESS;
}
