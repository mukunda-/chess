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

    // Center subraph
    int eval = 0;
    for (gc_edge_t* edge = graph->edges; edge != NULL; edge = edge->next) {
        /*
\exists c \in \text{opposite color pieces}:
               c \text{->} b \text{ with weight $> 1$}
\vee a \text{->} b \text{ with weight 0}
        */
        if (edge->a->color == GC_NODE_COLOR_WHITE && edge->b->id == SQUARE_D4) {
            eval += 1;
        } else if (edge->a->color == GC_NODE_COLOR_BLACK &&
                   edge->b->id == SQUARE_D4) {
            eval -= 1;
        }
    }

    fprintf(out_fp, "eval %d", eval);

    // Free resources
    movelist_free(moves);
    gc_graph_free(graph);
    board_free(board);

    return EXIT_SUCCESS;
}
