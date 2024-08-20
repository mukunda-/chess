#include "stockfish.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <pqxx/pqxx>
#include <sstream>
#include <string>

namespace bp = boost::process;

namespace stockfish {
void send_new_game(bp::opstream &os) { os << "ucinewgame" << std::endl; }

void send_eval(bp::opstream &os, const std::string &moves) {
  os << "position startpos moves " + moves << std::endl;
  os << "go depth 10" << std::endl;
}

void send_quit(bp::opstream &os) { os << "quit" << std::endl; }

void send_options(bp::opstream &os) {
  os << "uci" << std::endl;
  os << "setoption name Threads value 1" << std::endl;
  // os << "setoption name UCI_ShowWDL value true" << std::endl;
}

bool read_score_cp(bp::opstream &os, bp::ipstream &is, int *eval) {
  const std::string param_name = "score cp";

  os << "isready" << std::endl;

  bool found = false;
  std::string line;
  while (getline(is, line)) {
    if (line == "readyok") {
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

void read_response(bp::opstream &os, bp::ipstream &is) {
  os << "isready" << std::endl;

  std::string line;
  while (getline(is, line)) {
    std::cout << line << std::endl;
    if (line == "readyok") {
      return;
    }
  }
}

} // namespace stockfish
