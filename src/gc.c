#include "gc.h"

static const gc_direction_t KING_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_N,  GC_DIRECTION_NE, GC_DIRECTION_E,
    GC_DIRECTION_SE, GC_DIRECTION_S,  GC_DIRECTION_SW,
    GC_DIRECTION_W,  GC_DIRECTION_NW, GC_DIRECTION_NULL};

static const gc_direction_t QUEEN_DIRECTIONS[] = {
    GC_DIRECTION_N,  GC_DIRECTION_NE, GC_DIRECTION_E,
    GC_DIRECTION_SE, GC_DIRECTION_S,  GC_DIRECTION_SW,
    GC_DIRECTION_W,  GC_DIRECTION_NW, GC_DIRECTION_NULL};

static const gc_direction_t ROOK_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_N, GC_DIRECTION_E, GC_DIRECTION_S, GC_DIRECTION_W,
    GC_DIRECTION_NULL};

static const gc_direction_t BISHOP_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_NE, GC_DIRECTION_SE, GC_DIRECTION_SW, GC_DIRECTION_NW,
    GC_DIRECTION_NULL};

static const gc_direction_t EMPTY_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_NULL};

static const gc_direction_t KNIGHT_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_KNIGHT_N,  GC_DIRECTION_KNIGHT_NE, GC_DIRECTION_KNIGHT_E,
    GC_DIRECTION_KNIGHT_SE, GC_DIRECTION_KNIGHT_S,  GC_DIRECTION_KNIGHT_SW,
    GC_DIRECTION_KNIGHT_W,  GC_DIRECTION_KNIGHT_NW, GC_DIRECTION_NULL};

static const gc_direction_t UNIMPLEMENTED_DIRECTIONS[GC_DIRECTION_COUNT] = {
    GC_DIRECTION_NULL};

gc_node_t *gc_node_new(square_t id, gc_node_color_t color, gc_speed_t speed,
                       const gc_direction_t directions[GC_DIRECTION_COUNT],
                       const char *label) {
    gc_node_t* node = (gc_node_t*)malloc(sizeof(gc_node_t));
    assert(node != NULL && "Out of memory");
    memset(node, 0, sizeof(gc_node_t));

    node->id = id;
    if (label != NULL) {
        node->label = label;
    } else {
        node->label = SQUARE_NAMES[node->id];
    }
    node->speed = speed;
    node->directions = directions;
    node->color = color;

    return node;
}

void gc_node_free(gc_node_t *node) { free(node); }

gc_node_t *gc_node_king_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_ONE, KING_DIRECTIONS,
                       color == GC_NODE_COLOR_WHITE ? "♔" : "♚");
}

gc_node_t *gc_node_queen_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_MANY, QUEEN_DIRECTIONS,

                       color == GC_NODE_COLOR_WHITE ? "♕" : "♛");
}

gc_node_t *gc_node_rook_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_MANY, ROOK_DIRECTIONS,
                       color == GC_NODE_COLOR_WHITE ? "♖" : "♜");
}

gc_node_t *gc_node_pawn_new(square_t id, gc_node_color_t color) {
    // TODO check "id" to see if 7th 3rd rank and adjust accordingly.
    // TODO Add DOUBLE for movement
    return gc_node_new(id, color, GC_ONE, UNIMPLEMENTED_DIRECTIONS,
                       color == GC_NODE_COLOR_WHITE ? "♙" : "♟");
}

gc_node_t *gc_node_knight_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_ONE, KNIGHT_DIRECTIONS,
                       color == GC_NODE_COLOR_WHITE ? "♘" : "♞");
}

gc_node_t *gc_node_bishop_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_MANY, BISHOP_DIRECTIONS,
                       color == GC_NODE_COLOR_WHITE ? "♗" : "♝");
}
gc_node_t *gc_node_empty_new(square_t id, gc_node_color_t color) {
    return gc_node_new(id, color, GC_NONE, EMPTY_DIRECTIONS, SQUARE_NAMES[id]);
}
void gc_graph_free(gc_graph_t *graph);

gc_graph_t *gc_graph_new(board_t *board) {
    gc_graph_t* graph = (gc_graph_t*)malloc(sizeof(gc_graph_t));
    assert(graph != NULL && "Out of memory");
    memset(graph, 0, sizeof(gc_graph_t));

    for (square_t i = 0; i < SQUARE_COUNT; i++) {
        graph->hits[i] = 0;

        square_piece_t piece = board->squares[i];
        switch (piece) {
            case SQUARE_KING_WHITE:
            case SQUARE_KING_BLACK:
                graph->nodes[i] = gc_node_king_new(
                    i, piece == SQUARE_KING_WHITE ? GC_NODE_COLOR_WHITE
                                                  : GC_NODE_COLOR_BLACK);
                break;
            case SQUARE_QUEEN_WHITE:
            case SQUARE_QUEEN_BLACK:
                graph->nodes[i] = gc_node_queen_new(
                    i, piece == SQUARE_QUEEN_WHITE ? GC_NODE_COLOR_WHITE
                                                   : GC_NODE_COLOR_BLACK);
                break;
            case SQUARE_BISHOP_WHITE:
            case SQUARE_BISHOP_BLACK:
                graph->nodes[i] = gc_node_bishop_new(
                    i, piece == SQUARE_BISHOP_WHITE ? GC_NODE_COLOR_WHITE
                                                    : GC_NODE_COLOR_BLACK);
                break;
            case SQUARE_KNIGHT_WHITE:
            case SQUARE_KNIGHT_BLACK:
                graph->nodes[i] = gc_node_knight_new(
                    i, piece == SQUARE_KNIGHT_WHITE ? GC_NODE_COLOR_WHITE
                                                    : GC_NODE_COLOR_BLACK);
                break;
            case SQUARE_ROOK_WHITE:
            case SQUARE_ROOK_BLACK:
                graph->nodes[i] = gc_node_rook_new(
                    i, piece == SQUARE_ROOK_WHITE ? GC_NODE_COLOR_WHITE
                                                  : GC_NODE_COLOR_BLACK);
                break;
            case SQUARE_PAWN_WHITE:
            case SQUARE_PAWN_BLACK:
                graph->nodes[i] = gc_node_pawn_new(
                    i, piece == SQUARE_PAWN_WHITE ? GC_NODE_COLOR_WHITE
                                                  : GC_NODE_COLOR_BLACK);
                break;
            default:
                graph->nodes[i] = gc_node_empty_new(i, GC_NODE_COLOR_EMPTY);
                break;
        }
    }
    return graph;
}

void gc_fprint_node(FILE *out_fp, gc_node_t *node) {
    square_t square = node->id;

    dot_node_t dot_node = {.id = SQUARE_NAMES[square],
                           .label = node->label,
                           .pos_x = square % 8,
                           .pos_y = square / 8};

    dot_fprint_node(out_fp, dot_node);
}

void gc_fprint_edge(FILE *out_fp, gc_edge_t *edge) {
    const char *from = SQUARE_NAMES[edge->a->id];
    const char *to = SQUARE_NAMES[edge->b->id];

    const char *color = "black";
    if (edge->color == GC_EDGE_COLOR_ATTACK) {
        color = "red";
    } else if (edge->color == GC_EDGE_COLOR_DEFENSE) {
        color = "green";
    }

    dot_edge_t dot_edge = {.a = from, .b = to, .color = color};

    dot_fprint_edge(out_fp, dot_edge);
}

void gc_fprint_graph(FILE *out_fp, gc_graph_t *graph) {
    dot_fprint_start(out_fp);

    bool seen[SQUARE_COUNT] = {false};
    for (gc_edge_t *edge = graph->edges; edge != NULL; edge = edge->next) {
        if (graph->hits[edge->b->id] < 2 &&
            edge->b->color == GC_NODE_COLOR_EMPTY) {
            continue;
        }

        seen[edge->a->id] = true;
        seen[edge->b->id] = true;

        gc_fprint_edge(out_fp, edge);
    }

    for (square_t i = 0; i < SQUARE_COUNT; i++) {
        if (seen[i]) {
            gc_fprint_node(out_fp, graph->nodes[i]);
        }
    }

    dot_fprint_end(out_fp);
}

void gc_graph_insert_edges(gc_graph_t *graph, movelist_t *moves) {
    for (move_t *move = moves->head; move != NULL; move = move->next) {
        gc_graph_insert_edge(graph, move->from, move->to, 1);
    }
}

void gc_graph_insert_edge(gc_graph_t *graph, square_t a, square_t b,
                          int weight) {
    gc_edge_t* edge = (gc_edge_t*)malloc(sizeof(gc_edge_t));
    assert(edge != NULL && "Out of memory");
    memset(edge, 0, sizeof(gc_edge_t));

    edge->weight = weight;
    edge->a = graph->nodes[a];
    edge->b = graph->nodes[b];
    edge->next = graph->edges;

    graph->hits[edge->a->id]++;
    graph->hits[edge->b->id]++;

    if ((edge->a->color == GC_NODE_COLOR_WHITE &&
         edge->b->color == GC_NODE_COLOR_BLACK) ||
        (edge->b->color == GC_NODE_COLOR_WHITE &&
         edge->a->color == GC_NODE_COLOR_BLACK)) {
        edge->color = GC_EDGE_COLOR_ATTACK;
        edge->b->attacked_count++;
        edge->a->attacking_count++;
    } else if (edge->a->color == edge->b->color) {
        edge->color = GC_EDGE_COLOR_DEFENSE;
        edge->b->defended_count++;
        edge->a->defending_count++;
    } else {
        edge->color = GC_EDGE_COLOR_NEUTRAL;
    }

    edge->a->see_count++;
    edge->b->seen_count++;

    if (edge->a->color == GC_NODE_COLOR_WHITE) {
        edge->b->seen_white_count++;
    } else if (edge->a->color == GC_NODE_COLOR_WHITE) {
        edge->b->seen_black_count++;
    }

    graph->edges = edge;
}

void gc_edges_free(gc_edge_t *edges) {
    if (edges == NULL) {
        return;
    }

    gc_edges_free(edges->next);

    free(edges);
}

void gc_graph_free(gc_graph_t *graph) {
    gc_edges_free(graph->edges);

    for (square_t s = 0; s < SQUARE_COUNT; s++) {
        gc_node_free(graph->nodes[s]);
    }
}
