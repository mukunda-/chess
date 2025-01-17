#ifndef DOT_H
#define DOT_H

#include <stdio.h>

typedef struct dot_node {
    const char* id;
    const char* label;
    int pos_x;
    int pos_y;
} dot_node_t;

typedef struct dot_edge {
    const char* a;
    const char* b;
    const char* color;
} dot_edge_t;

void dot_fprint_edge(FILE* out_fp, dot_edge_t edge);

void dot_fprint_node(FILE* out_fp, dot_node_t node);

void dot_fprint_start(FILE* out_fp);

void dot_fprint_end(FILE* out_fp);

void dot_fprint_start_sub(FILE* out_fp, const char* name);

void dot_fprint_end_sub(FILE* out_fp);
#endif
