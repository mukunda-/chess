#ifndef MOVELIST_H
#define MOVELIST_H

typedef enum move_type_t {
  MOVE_TYPE_COMMENT = 0,
  MOVE_TYPE_VARIATION,
  MOVE_TYPE_MOVE,
  MOVE_TYPE_MOVE_NUMBER,
} move_type_t;

/**
 * In a PGN, each "move" in a move list are actually two moves
 * each followed by move notation and then optionally by
 * variations and comments.
 *
 */
typedef struct movelist_t {
  struct move_t *head;
  struct move_t *tail;
} movelist_t;

/**
 * Linked list of moves, variations, and comments from a movelist.
 *
 * Each node represents some subset of move, variation, comment.
 * Along, of course, with the next node in the list.
 */
typedef struct move_t {
  move_type_t kind;
  char *value;
  struct move_t *next;
} move_t;

/* Allocate a movelist_t */
movelist_t *movelist_new();

/* Dealocate movelist's memory */
void movelist_free(movelist_t *moves);

/* Add move number to movelist_t */
void movelist_add(movelist_t *moves, move_type_t kind, const char *value);

#endif
