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

    // Find the least active piece
    // Move it somewhere safe

    board_turn_t to_play = board->turn;
    gc_node_t* worst_piece = NULL;
    for (gc_edge_t* edge = graph->edges; edge != NULL; edge = edge->next) {
        gc_node_t* node = edge->a;
        if ((to_play == WHITE && node->color == GC_NODE_COLOR_WHITE) ||
            (to_play == BLACK && node->color == GC_NODE_COLOR_BLACK)) {
            if (worst_piece == NULL) {
                worst_piece = node;
            } else if (node->see_count < worst_piece->seen_count) {
                worst_piece = node;
            }
        }
    }

    // REDUCE TO EXHAUSTABLE PROPOSITIONS
    // maybe encapsulated in an enum?

    gc_node_t* best_dest = NULL;
    for (gc_edge_t* edge = graph->edges; edge != NULL; edge = edge->next) {
        gc_node_t* piece = edge->a;
        gc_node_t* dest = edge->b;

        if (piece->id != worst_piece->id) {
            continue;
        }

        if (piece->color == GC_NODE_COLOR_WHITE &&
            dest->seen_white_count <= piece->seen_white_count) {
            best_dest = dest;
        } else if (piece->color == GC_NODE_COLOR_BLACK &&
                   dest->seen_black_count <= piece->seen_black_count) {
            best_dest = dest;
        }
    }

    fprintf(out_fp, "%s%s\n", SQUARE_NAMES[worst_piece->id],
            SQUARE_NAMES[best_dest->id]);

    // Free resources
    movelist_free(moves);
    gc_graph_free(graph);
    board_free(board);

    return EXIT_SUCCESS;
}
