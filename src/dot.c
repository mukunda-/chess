#include "dot.h"

#include <stdio.h>

void dot_fprint_edge(FILE* out_fp, dot_edge_t edge) {
    fprintf(out_fp, "\t%s--%s\n", edge.a, edge.b);
}

void dot_fprint_node(FILE* out_fp, dot_node_t node) {
    fprintf(out_fp,
            "\t%s [fontsize=28 fixedsize=true shape=circle label=\"%s\" "
            "pos=\"%d,%d\" "
            "pin=true]\n",
            node.id, node.label, node.pos_x, node.pos_y);
}

void dot_fprint_start(FILE* out_fp) { fprintf(out_fp, "graph {\n"); }

void dot_fprint_end(FILE* out_fp) { fprintf(out_fp, "}\n"); }
