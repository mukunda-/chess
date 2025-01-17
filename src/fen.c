#include "fen.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "square.h"

square_piece_t to_piece(char symbol) {
    square_piece_t result = SQUARE_EMPTY;
    switch (symbol) {
        case 'P':
            result = SQUARE_PAWN_WHITE;
            break;
        case 'p':
            result = SQUARE_PAWN_BLACK;
            break;
        case 'N':
            result = SQUARE_KNIGHT_WHITE;
            break;
        case 'n':
            result = SQUARE_KNIGHT_BLACK;
            break;
        case 'B':
            result = SQUARE_BISHOP_WHITE;
            break;
        case 'b':
            result = SQUARE_BISHOP_BLACK;
            break;
        case 'R':
            result = SQUARE_ROOK_WHITE;
            break;
        case 'r':
            result = SQUARE_ROOK_BLACK;
            break;
        case 'Q':
            result = SQUARE_QUEEN_WHITE;
            break;
        case 'q':
            result = SQUARE_QUEEN_BLACK;
            break;
        case 'K':
            result = SQUARE_KING_WHITE;
            break;
        case 'k':
            result = SQUARE_KING_BLACK;
            break;
        default:
            break;
    }

    return result;
}

char from_piece(square_piece_t piece) {
    char result = '\0';
    switch (piece) {
        case SQUARE_PAWN_WHITE:
            result = 'P';
            break;
        case SQUARE_PAWN_BLACK:
            result = 'p';
            break;
        case SQUARE_KNIGHT_WHITE:
            result = 'N';
            break;
        case SQUARE_KNIGHT_BLACK:
            result = 'n';
            break;
        case SQUARE_BISHOP_WHITE:
            result = 'B';
            break;
        case SQUARE_BISHOP_BLACK:
            result = 'b';
            break;
        case SQUARE_ROOK_WHITE:
            result = 'R';
            break;
        case SQUARE_ROOK_BLACK:
            result = 'r';
            break;
        case SQUARE_QUEEN_WHITE:
            result = 'Q';
            break;
        case SQUARE_QUEEN_BLACK:
            result = 'q';
            break;
        case SQUARE_KING_WHITE:
            result = 'K';
            break;
        case SQUARE_KING_BLACK:
            result = 'k';
            break;
        default:
            break;
    }

    return result;
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
