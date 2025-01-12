#include "fen.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "square.h"

square_piece_t to_piece(char symbol) {
    switch (symbol) {
        case 'P':
            return SQUARE_PAWN_WHITE;
        case 'p':
            return SQUARE_PAWN_BLACK;
        case 'N':
            return SQUARE_KNIGHT_WHITE;
        case 'n':
            return SQUARE_KNIGHT_BLACK;
        case 'B':
            return SQUARE_BISHOP_WHITE;
        case 'b':
            return SQUARE_BISHOP_BLACK;
        case 'R':
            return SQUARE_ROOK_WHITE;
        case 'r':
            return SQUARE_ROOK_BLACK;
        case 'Q':
            return SQUARE_QUEEN_WHITE;
        case 'q':
            return SQUARE_QUEEN_BLACK;
        case 'K':
            return SQUARE_KING_WHITE;
        case 'k':
            return SQUARE_KING_BLACK;
        case '\0':
            return SQUARE_EMPTY;
    }

    return SQUARE_EMPTY;
}

char from_piece(square_piece_t piece) {
    switch (piece) {
        case SQUARE_PAWN_WHITE:
            return 'P';
        case SQUARE_PAWN_BLACK:
            return 'p';
        case SQUARE_KNIGHT_WHITE:
            return 'N';
        case SQUARE_KNIGHT_BLACK:
            return 'n';
        case SQUARE_BISHOP_WHITE:
            return 'B';
        case SQUARE_BISHOP_BLACK:
            return 'b';
        case SQUARE_ROOK_WHITE:
            return 'R';
        case SQUARE_ROOK_BLACK:
            return 'r';
        case SQUARE_QUEEN_WHITE:
            return 'Q';
        case SQUARE_QUEEN_BLACK:
            return 'q';
        case SQUARE_KING_WHITE:
            return 'K';
        case SQUARE_KING_BLACK:
            return 'k';
        case SQUARE_EMPTY:
            return '\0';
        case SQUARE_PIECE_COUNT:
            return '\0';
    }

    return '\0';
}

void fen_build(char **fen_out, board_t *board) {
    char *fen = malloc(100);
    size_t pos = 0;

    for (int rank = SQUARE_RANK_COUNT - 1; rank >= 0; rank--) {
        if (rank < SQUARE_RANK_COUNT - 1) {
            fen[pos++] = '/';
        }
        int empties = 0;
        for (int file = 0; file < SQUARE_FILE_COUNT; file++) {
            square_piece_t piece =
                board_get_piece(board, square_from(file, rank));
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
            break;
        }

        if (c == '/') {
            continue;
        }

        int rank = 7 - (idx / 8);
        int file = idx % 8;

        if (isdigit(c)) {
            int empties = c - '0';
            for (int i = 0; i < empties; i++) {
                board_set_piece(board, square_from(file + i, rank),
                                SQUARE_EMPTY);
                idx++;
            }
        } else {
            square_piece_t piece = to_piece(c);
            board_set_piece(board, square_from(file, rank), piece);
            idx++;
        }
    }

    if (*fen == 'w') {
        board->turn = WHITE;
    } else if (*fen == 'b') {
        board->turn = BLACK;
    }
}
