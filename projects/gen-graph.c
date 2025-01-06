#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "dot.h"
#include "fen.h"
#include "move.h"
#include "movegen.h"
#include "square.h"

void fprint_edges(FILE* out_fp, board_t* board, movelist_t* moves) {
    for (move_t* move = moves->head; move != NULL; move = move->next) {
        const char* from = SQUARE_NAMES[move->from];
        const char* to = SQUARE_NAMES[move->to];
        dot_edge_t edge = {.a = from, .b = to};

        dot_fprint_edge(out_fp, edge);
    }
}

int main(int argc, const char** argv) {
    FILE* out_fp = stdout;

    if (argc != 2) {
        printf("Usage: %s <FEN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* fen = argv[1];

    board_t* board = board_new();

    fen_parse(fen, board);

    dot_fprint_start(out_fp);

    for (size_t i = 0; i < SQUARE_COUNT; i++) {
        const char* id = SQUARE_NAMES[i];
        const char* label = "";
        if (board->squares[i] != SQUARE_EMPTY) {
            label = PIECE_NAMES[board->squares[i]];
        }

        dot_node_t node = {
            .id = id, .label = label, .pos_x = i % 8, .pos_y = i / 8};
        dot_fprint_node(out_fp, node);
    }

    movelist_t* moves = movelist_new();
    if (movegen(moves, board, WHITE)) {
        fprint_edges(out_fp, board, moves);
    } else {
        fprintf(stderr, "No legal moves!");
    }
    movelist_free(moves);

    moves = movelist_new();
    if (movegen(moves, board, BLACK)) {
        fprint_edges(out_fp, board, moves);
    } else {
        fprintf(stderr, "No legal moves!");
    }
    movelist_free(moves);

    dot_fprint_end(out_fp);

    return EXIT_SUCCESS;
}
