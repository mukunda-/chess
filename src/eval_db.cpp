#include "eval_db.h"

#include <cstdint>
#include <iostream>
#include <ostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>

#include "stockfish.h"

namespace bp = boost::process;

void read_games(bp::opstream &os, bp::ipstream &is, pqxx::connection &conn,
                std::ostream &log) {
  pqxx::work tx(conn);

  bool done = false;
  std::uint64_t last_id = 713176 - 1;
  while (!done) {
    done = true;
    for (const auto &[game_id] : tx.query<uint64_t>(
             "SELECT id FROM games where id > $1 order by id asc limit 1000",
             last_id)) {
      done = false;

      std::vector<std::tuple<std::string, std::uint64_t>> moves;
      for (const auto &[move, board_hash] :
           tx.query<std::string, std::uint64_t>(
               "SELECT move_uci, next_board_hash from continuations where "
               "game_id = $1 and ply <= 15 order by ply asc",
               game_id)) {
        moves.push_back(std::make_pair(move, board_hash));
      }

      stockfish::send_new_game(os);

      while (!moves.empty()) {
        std::string moves_concat;
        for (const auto &[move, board_hash] : moves) {
          moves_concat += " " + move;
        }

        stockfish::send_eval(os, moves_concat);
        const auto board_hash = std::get<1>(moves.at(moves.size() - 1));

        moves.pop_back();

        int eval = 0;
        if (stockfish::read_score_cp(os, is, &eval)) {
          tx.exec("update boards set stockfish_eval = $1 where hash = $2",
                  {eval, board_hash});
        }
      }

      last_id = game_id;
      log << game_id << std::endl;
      tx.commit();
    }
  }
}

int cmd_eval_db(std::ostream &out) {
  std::cerr << "Evaluating positions in DB" << std::endl;

  const char *dbUsername = std::getenv("DB_USERNAME");
  const char *dbPassword = std::getenv("DB_PASSWORD");
  const char *dbHost = std::getenv("DB_HOST");
  const char *dbName = std::getenv("DB_NAME");
  const char *dbPort = std::getenv("DB_PORT");

  if (!dbUsername || !dbPassword || !dbHost || !dbName || !dbPort) {
    std::cerr << "Environment variables DB_USERNAME, DB_PASSWORD, DB_NAME, "
                 "DB_PORT, or DB_HOST not set."
              << std::endl;
    return 1;
  }

  std::string conn_str =
      "host=" + std::string(dbHost) + " port=" + std::string(dbPort) +
      " user=" + std::string(dbUsername) +
      " password=" + std::string(dbPassword) + " dbname=" + std::string(dbName);

  pqxx::connection conn(conn_str);
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to database." << std::endl;

    return 1;
  }

  std::cerr << "Connected to database successfully." << std::endl;

  bp::ipstream is;
  bp::opstream os;

  bp::child c("./external/stockfish", bp::std_in<os, bp::std_out> is);

  stockfish::send_options(os);
  stockfish::read_response(os, is, std::cerr);

  read_games(os, is, conn, out);

  conn.close();

  return 0;
}
