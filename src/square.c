#include "square.h"

inline square_t square_from(square_file_t file, square_rank_t rank) {
    return file + (rank * SQUARE_RANK_COUNT);
}

inline square_file_t square_get_file(square_t square) {
    return square % SQUARE_FILE_COUNT;
}

inline square_rank_t square_get_rank(square_t square) {
    return square / SQUARE_RANK_COUNT;
}
