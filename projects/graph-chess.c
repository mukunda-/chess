#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "dot.h"
#include "fen.h"
#include "gc.h"
#include "move.h"
#include "movegen.h"
#include "square.h"

int main(int argc, const char** argv) {
    FILE* out_fp = stdout;

    if (argc != 2) {
        printf("Usage: %s <FEN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* fen = argv[1];

    board_t* board = board_new(WHITE);
    fen_parse(fen, board);
    gc_graph_t* graph = gc_graph_new(board);

    movelist_t* moves = movelist_new();
    movegen(moves, board, WHITE);
    movegen(moves, board, BLACK);

    // Add edges to graph
    gc_graph_insert_edges(graph, moves);

    // Print graph
    gc_fprint_graph(out_fp, graph);

    // Free resources
    movelist_free(moves);
    gc_graph_free(graph);
    board_free(board);

    return EXIT_SUCCESS;
}
