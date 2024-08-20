#include "eval_tsv.h"

#include <iostream>
#include <ostream>
#include <pqxx/pqxx>
#include <string>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>

#include "stockfish.h"

namespace bp = boost::process;

/*
void send_eval(bp::opstream &os, const std::string &moves) {
  stockfish::send_position(os, moves);
  stockfish::send_depth(os, 10);
}
*/

int cmd_eval_tsv(std::ifstream &in, std::ostream &out) {
  bp::ipstream is;
  bp::opstream os;

  bp::child c("./external/stockfish", bp::std_in<os, bp::std_out> is);

  stockfish::send_options(os);
  stockfish::read_response(os, is);

  stockfish::send_eval(os, "e2e4 b8c6");
  stockfish::read_response(os, is);

  stockfish::send_eval(os, "f2f3 e7e5 g2g4");
  stockfish::read_response(os, is);

  stockfish::send_eval(os, "f2f3 e7e5 g2g4 d8h4");
  stockfish::read_response(os, is);

  stockfish::send_quit(os);

  return 0;
}
