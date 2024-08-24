#include "eval_tsv.h"

#include <cctype>
#include <iostream>
#include <ostream>
#include <string>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/process/pipe.hpp>

#include <pqxx/pqxx>

#include "chess.hpp"
#include "stockfish.h"
#include "utils.h"

namespace bp = boost::process;

void send_eval(bp::opstream &os, bp::ipstream &is, const std::string &moves) {
  stockfish::send_position(os, moves);
  stockfish::send_depth(os, 20);
}

int cmd_eval_tsv(std::ifstream &in, std::ostream &out) {
  bp::ipstream is;
  bp::opstream os;

  bp::child c("./external/stockfish", bp::std_in<os, bp::std_out> is);

  stockfish::send_options(os);
  stockfish::read_response(os, is, std::cerr);

  std::vector<std::string> headers;
  if (!get_row(in, headers)) {
    std::cerr << "Missing header row" << std::endl;
    return 1;
  }

  int pgn_idx = get_header_index(headers, "pgn");
  if (pgn_idx < 0) {
    std::cerr << "Header 'pgn' not found" << std::endl;
    return 1;
  }

  int name_idx = get_header_index(headers, "name");
  if (name_idx < 0) {
    std::cerr << "Header 'name' not found" << std::endl;
    return 1;
  }

  std::vector<std::string> row;
  std::string line;
  while (get_row(in, row)) {
    std::vector<std::string> moves;
    split(row.at(pgn_idx), moves, ' ');

    chess::Board board;
    std::string uci = "";
    for (const auto &san : moves) {
      if (san.empty() || isdigit(san.at(0))) {
        continue;
      }
      const chess::Move move = chess::uci::parseSan(board, san);
      board.makeMove(move);
      uci += chess::uci::moveToUci(move) + " ";
    }

    trim(uci);

    stockfish::Eval eval;
    send_eval(os, is, uci);
    if (!stockfish::read_eval(is, eval)) {
      out << row.at(name_idx) << "\t" << "?" << "\t" << "?" << "\t" << "?"
          << "\t" << "?" << "\t" << "?" << std::endl;
    } else {
      out << row.at(name_idx) << "\t" << eval.cp << "\t" << eval.win << "\t"
          << eval.draw << "\t" << eval.loss << "\t" << uci << std::endl;
    }
  }

  stockfish::send_quit(os);

  return 0;
}
