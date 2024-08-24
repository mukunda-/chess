#pragma once

#include <iostream>

#include <boost/process.hpp>

namespace stockfish {

struct Eval {
  int win;
  int draw;
  int loss;
  int cp;
  std::string best_move;
};

void send_position(boost::process::opstream &os, const std::string &moves);

void send_position_fen(boost::process::opstream &os, const std::string &fen);

void send_depth(boost::process::opstream &os, unsigned short d);

void send_new_game(boost::process::opstream &os);

void send_eval(boost::process::opstream &os, const std::string &moves);

void send_quit(boost::process::opstream &os);

void send_options(boost::process::opstream &os);

void read_response(boost::process::opstream &os, boost::process::ipstream &is,
                   std::ostream &out);

bool read_eval(boost::process::ipstream &is, Eval &eval);

bool read_score_cp(boost::process::opstream &os, boost::process::ipstream &is,
                   int *eval);
} // namespace stockfish
