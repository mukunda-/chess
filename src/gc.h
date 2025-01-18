#ifndef GC_H
#define GC_H

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "square.h"

typedef enum gc_node_color {
    GC_NODE_COLOR_BLACK = BLACK,
    GC_NODE_COLOR_WHITE = WHITE,
    GC_NODE_COLOR_EMPTY
} gc_node_color_t;

typedef enum gc_edge_color {
    GC_EDGE_COLOR_DEFENSE,
    GC_EDGE_COLOR_ATTACK,
    GC_EDGE_COLOR_NEUTRAL
} gc_edge_color_t;

typedef enum gc_direction {
    GC_DIRECTION_N = 0,
    GC_DIRECTION_NE,
    GC_DIRECTION_E,
    GC_DIRECTION_SE,
    GC_DIRECTION_S,
    GC_DIRECTION_SW,
    GC_DIRECTION_W,
    GC_DIRECTION_NW,
    GC_DIRECTION_KNIGHT_N,
    GC_DIRECTION_KNIGHT_NE,
    GC_DIRECTION_KNIGHT_E,
    GC_DIRECTION_KNIGHT_SE,
    GC_DIRECTION_KNIGHT_S,
    GC_DIRECTION_KNIGHT_SW,
    GC_DIRECTION_KNIGHT_W,
    GC_DIRECTION_KNIGHT_NW,
    GC_DIRECTION_COUNT,
    GC_DIRECTION_NULL
} gc_direction_t;

typedef enum gc_speed {
    GC_ONE = 0,
    GC_MANY,
    GC_NONE,
} gc_speed_t;

typedef struct gc_node {
    gc_speed_t speed;
    const gc_direction_t *directions;
    size_t directions_count;
    square_t id;
    gc_node_color_t color;
    const char *label;
    size_t attacked_count;
    size_t attacking_count;
    size_t defended_count;
    size_t defending_count;
} gc_node_t;

typedef struct gc_edge {
    gc_node_t *a;
    gc_node_t *b;
    struct gc_edge *next;
    gc_edge_color_t color;
} gc_edge_t;

typedef struct gc_graph {
    gc_node_t *nodes[SQUARE_COUNT];
    gc_edge_t *edges;
    size_t weights[SQUARE_COUNT][SQUARE_COUNT];
} gc_graph_t;

gc_graph_t *gc_graph_new(board_t *board);

void gc_graph_free(gc_graph_t *graph);

gc_node_t *gc_node_new(square_t id, gc_node_color_t color, gc_speed_t speed,
                       const gc_direction_t directions[GC_DIRECTION_COUNT],
                       const char *label);
gc_edge_t *gc_edge_new(gc_node_t *a, gc_node_t *b);

void gc_graph_insert_edge(gc_graph_t *graph, square_t a, square_t b);

void gc_graph_insert_edges(gc_graph_t *graph, movelist_t *moves);

gc_node_t *gc_node_king_new(square_t id, gc_node_color_t color);
gc_node_t *gc_node_queen_new(square_t id, gc_node_color_t color);
gc_node_t *gc_node_rook_new(square_t id, gc_node_color_t color);
gc_node_t *gc_node_bishop_new(square_t id, gc_node_color_t color);
gc_node_t *gc_node_knight_new(square_t id, gc_node_color_t color);
gc_node_t *gc_node_pawn_new(square_t id, gc_node_color_t color);

void gc_free_graph(gc_graph_t *graph);

void gc_fprint_graph(FILE *out_fp, gc_graph_t *graph);

#endif
