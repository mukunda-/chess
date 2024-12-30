#include "square.h"

square_t square_from(square_file_t file, square_rank_t rank) {
    return file + (rank * SQUARE_RANK_COUNT);
}
