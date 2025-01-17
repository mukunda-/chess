#include "dot.h"

#include <stdio.h>

void dot_fprint_edge(FILE* out_fp, dot_edge_t edge) {
    fprintf(out_fp, "\t%s->%s [color=%s]\n", edge.a, edge.b, edge.color);
}

void dot_fprint_node(FILE* out_fp, dot_node_t node) {
    fprintf(out_fp,
            "\t%s [shape=circle label=\"%s\" "
            "]\n",
            node.id, node.label, node.pos_x, node.pos_y);
}

void dot_fprint_start(FILE* out_fp) {
    fprintf(out_fp, "digraph {\noverlap=false;\nscale=true;\n");
}

void dot_fprint_start_sub(FILE* out_fp, const char* name) {
    fprintf(out_fp, "subgraph cluster_%s {", name);
}

void dot_fprint_end(FILE* out_fp) { fprintf(out_fp, "}\n"); }

void dot_fprint_end_sub(FILE* out_fp) { fprintf(out_fp, "}\n"); }
