typedef struct board_move {
    struct board_move *next;
} board_move_t;

typedef struct board_movelist {
    board_move_t *head;
    board_move_t *tail;
} board_movelist_t;

/* Allocate a board_movelist_t */
board_movelist_t *board_movelist_new();

/* Dealocate board_movelist's memory */
void board_movelist_free(board_movelist_t *board_moves);
