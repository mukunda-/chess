#include "stockfish.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <pqxx/pqxx>
#include <sstream>
#include <string>

#include <boost/asio.hpp>
#include <boost/process.hpp>

#include "utils.h"

namespace bp = boost::process;

namespace stockfish {

void send(bp::opstream &os, const std::string &msg) { os << msg << std::endl; }

void send_new_game(bp::opstream &os) { send(os, "ucinewgame"); }

void send_position(bp::opstream &os, const std::string &moves) {
  send(os, "position startpos moves " + moves);
}

void send_position_fen(bp::opstream &os, const std::string &fen) {
  send(os, "position fen " + fen);
}

void send_depth(bp::opstream &os, unsigned short d) {
  send(os, "go depth " + std::to_string(d));
}

void send_eval(bp::opstream &os, const std::string &moves) {
  send_position(os, moves);
  send_depth(os, 10);
}

void send_quit(bp::opstream &os) { send(os, "quit"); }

void send_options(bp::opstream &os) {
  send(os, "uci");
  send(os, "setoption name Threads value 1");
  send(os, "setoption name UCI_ShowWDL value true");
}

bool read_eval(bp::ipstream &is, Eval &eval) {
  bool found = false;
  std::string line;
  while (getline(is, line)) {
    if (line.starts_with("bestmove ")) {
      break;
    }

    std::vector<std::string> fields;
    split(line, fields, ' ');

    for (std::size_t i = 0; i < fields.size(); i++) {
      if (fields[i] == "wdl") {
        eval.win = std::stoi(fields[i + 1]);
        eval.draw = std::stoi(fields[i + 2]);
        eval.loss = std::stoi(fields[i + 3]);

        found = true;
        i += 3;
      } else if (fields[i] == "score" && fields[i + 1] == "cp") {
        eval.cp = std::stoi(fields[i + 2]);
        i += 2;
      }
    }
  }

  return found;
}

bool read_score_cp(bp::opstream &os, bp::ipstream &is, int *eval) {
  const std::string param_name = "score cp";

  bool found = false;
  std::string line;
  while (getline(is, line)) {
    if (line.starts_with("bestmove ")) {
      break;
    }

    const auto idx = line.find(param_name);
    if (idx == std::string::npos) {
      continue;
    }
    std::string chunk = line.substr(idx + param_name.size());
    std::stringstream ss(chunk);
    std::istringstream(chunk) >> *eval;
    found = true;
  }

  return found;
}

void read_response(bp::opstream &os, bp::ipstream &is, std::ostream &out) {
  send(os, "isready");

  std::string line;
  while (getline(is, line)) {
    out << line << std::endl;
    if (line == "readyok") {
      return;
    }
  }
}

} // namespace stockfish
