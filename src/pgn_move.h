#ifndef PGN_MOVE_H
#define PGN_MOVE_H

typedef enum pgn_movetype {
    PGN_MOVETYPE_VARIATION,
    PGN_MOVETYPE_MOVE,
    PGN_MOVETYPE_MOVE_NUMBER,
    PGN_MOVETYPE_CLOCK,
} pgn_movetype_t;

/**
 * In a PGN, each "move" in a move list are actually two moves
 * each followed by move notation and then optionally by
 * variations and comments.
 *
 */
typedef struct pgn_movelist {
    struct pgn_move *head;
    struct pgn_move *tail;
} pgn_movelist_t;

/**
 * Linked list of moves, variations, and comments from a movelist.
 *
 * Each node represents some subset of move, variation, comment.
 * Along, of course, with the next node in the list.
 */
typedef struct pgn_move {
    pgn_movetype_t kind;
    char *value;
    struct pgn_move *next;
} pgn_move_t;

/* Allocate a pgn_movelist_t */
pgn_movelist_t *pgn_movelist_new(void);

/* Dealocate movelist's memory */
void pgn_movelist_free(pgn_movelist_t *moves);

/* Add move number to pgn_movelist_t */
void pgn_movelist_add(pgn_movelist_t *moves, pgn_movetype_t kind,
                      const char *value);

#endif
