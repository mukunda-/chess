#ifndef DISPLAY_H
#define DISPLAY_H

#include "global.h"
#include "pgn.h"

void print_pgn(tagspec_t *spec, pgn_t *pgn);
void print_headers(tagspec_t *spec);

#endif /* DISPLAY_H */
