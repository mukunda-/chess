#include "fen.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"

board_piece_t to_piece(char symbol) {
    switch (symbol) {
        case 'P':
            return BOARD_PIECE_PAWN_WHITE;
        case 'p':
            return BOARD_PIECE_PAWN_BLACK;
        case 'N':
            return BOARD_PIECE_KNIGHT_WHITE;
        case 'n':
            return BOARD_PIECE_KNIGHT_BLACK;
        case 'B':
            return BOARD_PIECE_BISHOP_WHITE;
        case 'b':
            return BOARD_PIECE_BISHOP_BLACK;
        case 'R':
            return BOARD_PIECE_ROOK_WHITE;
        case 'r':
            return BOARD_PIECE_ROOK_BLACK;
        case 'Q':
            return BOARD_PIECE_QUEEN_WHITE;
        case 'q':
            return BOARD_PIECE_QUEEN_BLACK;
        case 'K':
            return BOARD_PIECE_KING_WHITE;
        case 'k':
            return BOARD_PIECE_KING_BLACK;
        case '\0':
            return BOARD_PIECE_EMPTY;
    }

    return BOARD_PIECE_EMPTY;
}

char from_piece(board_piece_t piece) {
    switch (piece) {
        case BOARD_PIECE_PAWN_WHITE:
            return 'P';
        case BOARD_PIECE_PAWN_BLACK:
            return 'p';
        case BOARD_PIECE_KNIGHT_WHITE:
            return 'N';
        case BOARD_PIECE_KNIGHT_BLACK:
            return 'n';
        case BOARD_PIECE_BISHOP_WHITE:
            return 'B';
        case BOARD_PIECE_BISHOP_BLACK:
            return 'b';
        case BOARD_PIECE_ROOK_WHITE:
            return 'R';
        case BOARD_PIECE_ROOK_BLACK:
            return 'r';
        case BOARD_PIECE_QUEEN_WHITE:
            return 'Q';
        case BOARD_PIECE_QUEEN_BLACK:
            return 'q';
        case BOARD_PIECE_KING_WHITE:
            return 'K';
        case BOARD_PIECE_KING_BLACK:
            return 'k';
        case BOARD_PIECE_EMPTY:
            return '\0';
    }

    return '\0';
}

void fen_build(char **fen_out, board_t *board) {
    char *fen = malloc(100);
    size_t pos = 0;

    for (int rank = BOARD_RANK_COUNT - 1; rank >= 0; rank--) {
        if (rank < BOARD_RANK_COUNT - 1) {
            fen[pos++] = '/';
        }
        int empties = 0;
        for (int file = 0; file < BOARD_FILE_COUNT; file++) {
            board_piece_t piece = board_get_piece(board, file, rank);
            char symbol = from_piece(piece);
            if (symbol != '\0') {
                if (empties > 0) {
                    fen[pos++] = '0' + empties;
                    empties = 0;
                }
                fen[pos++] = symbol;
            } else {
                empties++;
            }
        }
        if (empties > 0) {
            fen[pos++] = '0' + empties;
            empties = 0;
        }
    }

    // Turn
    pos += sprintf(&fen[pos], " %c", board->turn == WHITE ? 'w' : 'b');

    // Castling rights
    pos += sprintf(&fen[pos], " KQkq");

    // e.p. target square
    pos += sprintf(&fen[pos], " -");

    // Move/ply
    pos += sprintf(&fen[pos], " 0 1");

    fen[pos++] = '\0';

    *fen_out = fen;
}

void fen_parse(const char *fen, board_t *board) {
    int idx = 0;
    while (*fen != '\0') {
        char c = *(fen++);
        if (c == ' ') {
            return;
        }

        if (c == '/') {
            continue;
        }

        int rank = 7 - (idx / 8);
        int file = idx % 8;

        if (isdigit(c)) {
            int empties = c - '0';
            for (int i = 0; i < empties; i++) {
                board_set_piece(board, file + i, rank, BOARD_PIECE_EMPTY);
                idx++;
            }
        } else {
            board_piece_t piece = to_piece(c);
            board_set_piece(board, file, rank, piece);
            idx++;
        }
    }
}
