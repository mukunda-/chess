#include <iostream>

#include "chess.hpp"

int cmd_classify_endgames(std::ifstream &in, std::ostream &out);
std::string classify_endgame(const chess::Board &board);
