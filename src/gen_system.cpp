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
#include <tuple>

#include "chess.hpp"
#include "stockfish.h"

namespace bp = boost::process;

void get_moves(std::ostream &out, bp::opstream &os, bp::ipstream &is,
               const std::string &fen, const chess::Color player,
               const std::string &so_far, const int max_depth) {
  if (max_depth <= 0) {
    return;
  }

  chess::Board board(fen);
  const bool my_move = player == board.sideToMove();

  chess::movegen gen;
  chess::Movelist legal;
  gen.legalmoves(legal, board);

  std::vector<std::tuple<std::string, std::string>> moves;
  for (const auto &move : legal) {
    board.makeMove(move);

    const auto uci = chess::uci::moveToUci(move);
    auto game = so_far;
    if (!so_far.empty()) {
      game += " ";
    }

    game += uci;

    stockfish::Eval eval;
    stockfish::send_position(os, game);
    stockfish::send_depth(os, 10);
    if (!stockfish::read_eval(is, eval)) {
      std::cerr << "Missing eval for " << game << std::endl;
      continue;
    }

    if (my_move) {
      const int tolerance = 30;
      const bool good_for_me = player == chess::Color::WHITE
                                   ? eval.cp > -tolerance
                                   : eval.cp < tolerance;

      if (good_for_me) {
        const auto next_fen = board.getFen();
        moves.push_back(std::make_tuple(next_fen, game));
      } else {
        std::cerr << "Pruning " << game << std::endl;
      }
    } else {
      const int tolerance = 1000;
      const bool good_for_me = player == chess::Color::WHITE
                                   ? eval.cp > -tolerance
                                   : eval.cp < tolerance;

      if (!good_for_me) {
        return;
      }

      const auto next_fen = board.getFen();
      moves.push_back(std::make_tuple(next_fen, game));
    }

    board.unmakeMove(move);
  }

  for (const auto &[next_fen, game] : moves) {
    get_moves(out, os, is, next_fen, player, game, max_depth - 1);
  }
}

int cmd_gen_system(std::ostream &out) {
  bp::ipstream is;
  bp::opstream os;

  bp::child c("./external/stockfish", bp::std_in<os, bp::std_out> is);

  stockfish::send_options(os);

  std::map<std::string, std::vector<std::string>> tree;

  chess::Board board;
  out << "For white" << std::endl;
  get_moves(out, os, is, board.getFen(), chess::Color::WHITE, "", 3);

  stockfish::send_quit(os);

  return 0;
}
