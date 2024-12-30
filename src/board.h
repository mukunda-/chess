#ifndef BOARD_H
#define BOARD_H

#define BOARD_RANK_COUNT 8
#define BOARD_FILE_COUNT 8
#define BOARD_SQUARE_COUNT BOARD_RANK_COUNT *BOARD_FILE_COUNT

typedef enum board_turn {
    WHITE = 0,
    BLACK,
} board_turn_t;

typedef enum board_rank {
    BOARD_RANK_1 = 0,
    BOARD_RANK_2,
    BOARD_RANK_3,
    BOARD_RANK_4,
    BOARD_RANK_5,
    BOARD_RANK_6,
    BOARD_RANK_7,
    BOARD_RANK_8,
    BOARD_RANK_OFF_BOARD,
} board_rank_t;

typedef enum board_file {
    BOARD_FILE_A = 0,
    BOARD_FILE_B,
    BOARD_FILE_C,
    BOARD_FILE_D,
    BOARD_FILE_E,
    BOARD_FILE_F,
    BOARD_FILE_G,
    BOARD_FILE_H,
    BOARD_FILE_OFF_BOARD,
} board_file_t;

typedef enum board_piece {
    BOARD_PIECE_EMPTY = 0,
    BOARD_PIECE_PAWN_WHITE,
    BOARD_PIECE_PAWN_BLACK,
    BOARD_PIECE_KNIGHT_WHITE,
    BOARD_PIECE_KNIGHT_BLACK,
    BOARD_PIECE_BISHOP_WHITE,
    BOARD_PIECE_BISHOP_BLACK,
    BOARD_PIECE_ROOK_WHITE,
    BOARD_PIECE_ROOK_BLACK,
    BOARD_PIECE_QUEEN_WHITE,
    BOARD_PIECE_QUEEN_BLACK,
    BOARD_PIECE_KING_WHITE,
    BOARD_PIECE_KING_BLACK,
} board_piece_t;

typedef struct board {
    board_piece_t squares[BOARD_SQUARE_COUNT];
    board_turn_t turn;
} board_t;

board_t *board_new(void);

void board_free(board_t *board);

void board_print(board_t *board);

void board_set_piece(board_t *board, board_file_t file, board_rank_t rank,
                     board_piece_t piece);

board_piece_t board_get_piece(board_t *board, board_file_t file,
                              board_rank_t rank);

board_rank_t board_get_rank(board_t *board, board_piece_t piece,
                            board_file_t file);
board_file_t board_get_file(board_t *board, board_piece_t piece,
                            board_rank_t rank);

#endif
