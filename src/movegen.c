#include "movegen.h"

bool insert_move(movelist_t* moves_out, board_t* board, square_t from,
                 square_t to) {
    if (from == SQUARE_OOB || to == SQUARE_OOB) {
        return false;
    }

    move_t* move = move_new(from, to);
    move->capture = board->squares[to] != SQUARE_EMPTY;

    movelist_insert_end(moves_out, move);

    return !move->capture;
}

bool movegen(movelist_t* out_moves, board_t* board, board_turn_t turn) {
    for (size_t i = 0; i < SQUARE_COUNT; i++) {
        square_piece_t piece = board->squares[i];
        if (turn == WHITE) {
            switch (piece) {
                case SQUARE_KING_WHITE:
                    movegen_king(out_moves, board, i);
                    break;
                case SQUARE_QUEEN_WHITE:
                    movegen_queen(out_moves, board, i);
                    break;
                case SQUARE_BISHOP_WHITE:
                    movegen_bishop(out_moves, board, i);
                    break;
                case SQUARE_KNIGHT_WHITE:
                    movegen_knight(out_moves, board, i);
                    break;
                case SQUARE_ROOK_WHITE:
                    movegen_rook(out_moves, board, i);
                    break;
                case SQUARE_PAWN_WHITE:
                    movegen_pawn(out_moves, board, i);
                    break;
                default:
                    break;
            }
        } else {
            switch (piece) {
                case SQUARE_KING_BLACK:
                    movegen_king(out_moves, board, i);
                    break;
                case SQUARE_QUEEN_BLACK:
                    movegen_queen(out_moves, board, i);
                    break;
                case SQUARE_BISHOP_BLACK:
                    movegen_bishop(out_moves, board, i);
                    break;
                case SQUARE_KNIGHT_BLACK:
                    movegen_knight(out_moves, board, i);
                    break;
                case SQUARE_ROOK_BLACK:
                    movegen_rook(out_moves, board, i);
                    break;
                case SQUARE_PAWN_BLACK:
                    movegen_pawn(out_moves, board, i);
                    break;
                default:
                    break;
            }
        }
    }

    return out_moves->head != NULL;
}

movelist_t* movegen_subgraph(movelist_t* moves, square_t center) {
    movelist_t* out = movelist_new();

    square_t to_squares[SQUARE_COUNT];
    square_t from_squares[SQUARE_COUNT];

    for (move_t* edge = moves->head; edge != NULL; edge = edge->next) {
        to_squares[edge->from] = edge->to;
        from_squares[edge->to] = edge->from;
    }

    return out;
}

void movegen_bishop(movelist_t* moves_out, board_t* board, square_t from) {
    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank + i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank - i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank + i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank - i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }
}

void movegen_king(movelist_t* moves_out, board_t* board, square_t from) {
    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    insert_move(moves_out, board, from, square_from(file, rank + 1));
    insert_move(moves_out, board, from, square_from(file, rank - 1));
    insert_move(moves_out, board, from, square_from(file + 1, rank));
    insert_move(moves_out, board, from, square_from(file + 1, rank + 1));
    insert_move(moves_out, board, from, square_from(file + 1, rank - 1));
    insert_move(moves_out, board, from, square_from(file - 1, rank));
    insert_move(moves_out, board, from, square_from(file - 1, rank + 1));
    insert_move(moves_out, board, from, square_from(file - 1, rank - 1));
}

void movegen_pawn(movelist_t* moves_out, board_t* board, square_t from) {
    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    bool is_black = board->squares[from] == SQUARE_PAWN_BLACK;
    if (is_black && rank == SQUARE_RANK_7) {
        square_t double_jump = square_from(file, rank - 2);
        insert_move(moves_out, board, from, double_jump);
    } else if (!is_black && rank == SQUARE_RANK_2) {
        square_t double_jump = square_from(file, rank + 2);
        insert_move(moves_out, board, from, double_jump);
    }

    int up_rank = rank + (is_black ? -1 : 1);
    square_t up = square_from(file, up_rank);
    if (up != SQUARE_OOB && board->squares[up] == SQUARE_EMPTY) {
        insert_move(moves_out, board, from, up);
    }

    square_t left = square_from(file - 1, up_rank);
    if (left != SQUARE_OOB && board->squares[left] == SQUARE_EMPTY) {
        insert_move(moves_out, board, from, left);
    }

    square_t right = square_from(file + 1, up_rank);
    if (right != SQUARE_OOB && board->squares[right] == SQUARE_EMPTY) {
        insert_move(moves_out, board, from, right);
    }
}

void movegen_knight(movelist_t* moves_out, board_t* board, square_t from) {
    square_file_t file = square_get_file(from);
    square_rank_t rank = square_get_rank(from);

    insert_move(moves_out, board, from, square_from(file + 1, rank + 2));
    insert_move(moves_out, board, from, square_from(file - 1, rank + 2));

    insert_move(moves_out, board, from, square_from(file + 1, rank - 2));
    insert_move(moves_out, board, from, square_from(file - 1, rank - 2));

    insert_move(moves_out, board, from, square_from(file + 2, rank + 1));
    insert_move(moves_out, board, from, square_from(file - 2, rank + 1));

    insert_move(moves_out, board, from, square_from(file + 2, rank - 1));
    insert_move(moves_out, board, from, square_from(file - 2, rank - 1));
}

void movegen_queen(movelist_t* moves_out, board_t* board, square_t from) {
    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank + i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank - i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank + i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank - i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, rank + i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, rank - i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file - i, rank);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file + i, rank);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }
}

void movegen_rook(movelist_t* moves_out, board_t* board, square_t from) {
    square_rank_t rank = square_get_rank(from);
    square_file_t file = square_get_file(from);

    for (int i = rank + 1; i < SQUARE_RANK_COUNT; i++) {
        square_t to = square_from(file, i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = rank - 1; i >= 0; i--) {
        square_t to = square_from(file, i);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = file + 1; i < SQUARE_FILE_COUNT; i++) {
        square_t to = square_from(i, rank);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }

    for (int i = file - 1; i >= 0; i--) {
        square_t to = square_from(i, rank);
        if (!insert_move(moves_out, board, from, to)) {
            break;
        }
    }
}
