#include "intake.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "chess.hpp"
#include "utils.h"

#define SEP '\t'

#include "debug_time.h"

DEBUG_TIME_DECLARE(flush);
DEBUG_TIME_DECLARE(game_work);
DEBUG_TIME_DECLARE(endgame_work);
DEBUG_TIME_DECLARE(board_work);
DEBUG_TIME_DECLARE(continuation_work);
DEBUG_TIME_DECLARE(commit);

#define LIGHT_SQUARES_MASK 0x55AA55AA55AA55AA
#define DARK_SQUARES_MASK 0xAA55AA55AA55AA55

struct EndgameClassComp {
  chess::PieceType piece_type;
  chess::Color color;
  char out;
};

std::string classify_endgame(const chess::Board &board) {
  std::map<char, int> counts;

  constexpr EndgameClassComp endgameComps[12] = {
      {chess::PieceType::KING, chess::Color::WHITE, 'K'},
      {chess::PieceType::BISHOP, chess::Color::WHITE, 'B'},
      {chess::PieceType::KNIGHT, chess::Color::WHITE, 'N'},
      {chess::PieceType::ROOK, chess::Color::WHITE, 'R'},
      {chess::PieceType::QUEEN, chess::Color::WHITE, 'Q'},
      {chess::PieceType::PAWN, chess::Color::WHITE, 'P'},
      {chess::PieceType::KING, chess::Color::BLACK, 'K'},
      {chess::PieceType::BISHOP, chess::Color::BLACK, 'B'},
      {chess::PieceType::KNIGHT, chess::Color::BLACK, 'N'},
      {chess::PieceType::ROOK, chess::Color::BLACK, 'R'},
      {chess::PieceType::QUEEN, chess::Color::BLACK, 'Q'},
      {chess::PieceType::PAWN, chess::Color::BLACK, 'P'}};
  constexpr std::array<uint64_t, 2> square_masks = {LIGHT_SQUARES_MASK,
                                                    DARK_SQUARES_MASK};

  std::string out;
  int total_pieces = 0;
  for (const auto &comp : endgameComps) {
    const auto bitboard = board.pieces(comp.piece_type, comp.color);

    auto count = bitboard.count();
    total_pieces += count;
    if (total_pieces > 5) {
      return "";
    }

    for (int j = 0; j < count; j++) {
      out += comp.out;
    }
  }

  std::string out_bishops;
  for (const auto color : {chess::Color::WHITE, chess::Color::BLACK}) {
    const auto bitboard = board.pieces(chess::PieceType::BISHOP, color);
    for (const uint64_t square_mask : square_masks) {
      out_bishops += std::to_string((bitboard & square_mask).count());
    }
  }

  if (out_bishops != "0000") {
    out += "_" + out_bishops;
  }

  return out;
}

struct Board {
  std::uint64_t white_bishops;
  std::uint64_t white_rooks;
  std::uint64_t white_queens;
  std::uint64_t white_knights;
  std::uint64_t white_king;
  std::uint64_t white_pawn;
  std::uint64_t black_bishops;
  std::uint64_t black_rooks;
  std::uint64_t black_queens;
  std::uint64_t black_knights;
  std::uint64_t black_king;
  std::uint64_t black_pawn;
  bool white_to_move;
  std::string castle_rights;
  int enpassant_sq;
  std::uint64_t hash;
};

struct Continuation {
  Board board;
  std::string move_uci;
  std::uint64_t next_board_hash;
  int ply;
};

struct Game {
  std::string white = "?";
  std::string white_elo = "?";
  std::string white_fide_id = "?";
  std::string white_rating_diff = "?";
  std::string white_team = "?";
  std::string white_title = "?";
  std::string black = "?";
  std::string black_elo = "?";
  std::string black_fide_id = "?";
  std::string black_rating_diff = "?";
  std::string black_team = "?";
  std::string black_title = "?";
  std::string annotator = "?";
  std::string board = "?";
  std::string date = "?";
  std::string eco = "?";
  std::string event = "?";
  std::string opening = "?";
  std::string result = "?";
  std::string round = "?";
  std::string site = "?";
  std::string termination = "?";
  std::string time_control = "?";
  std::string utc_date = "?";
  std::string utc_time = "?";
  std::vector<Continuation> continuations;
  std::vector<std::string> endgames;
  int ply = 0;
};

void insert_games(pqxx::work &tx, const std::vector<Game> &games,
                  std::map<std::string, std::int64_t> &game_ids) {
  const pqxx::table_path games_table_path = {"games"};

  auto games_table = pqxx::stream_to::table(
      tx, games_table_path, {"white",         "white_elo",
                             "white_fide_id", "white_rating_diff",
                             "white_team",    "white_title",
                             "black",         "black_elo",
                             "black_fide_id", "black_rating_diff",
                             "black_team",    "black_title",
                             "annotator",     "board",
                             "date",          "eco",
                             "event",         "opening",
                             "result",        "round",
                             "site",          "termination",
                             "time_control",  "utc_date",
                             "utc_time"});

  for (auto const &game : games) {
    if (game_ids.count(game.site) > 0) {
      continue;
    }

    auto row = std::make_tuple(
        game.white, game.white_elo, game.white_fide_id, game.white_rating_diff,
        game.white_team, game.white_title, game.black, game.black_elo,
        game.black_fide_id, game.black_rating_diff, game.black_team,
        game.black_title, game.annotator, game.board, game.date, game.eco,
        game.event, game.opening, game.result, game.round, game.site,
        game.termination, game.time_control, game.utc_date, game.utc_time);

    games_table << row;
  }

  games_table.complete();
}

void select_game_ids(pqxx::work &tx, const std::vector<Game> &games,
                     std::map<std::string, std::int64_t> &game_ids) {
  std::string games_query;
  pqxx::placeholders games_placeholders;
  pqxx::params games_params;

  for (auto const &game : games) {
    if (games_query == "") {
      games_query = "SELECT id, site from games where site in (" +
                    games_placeholders.get();
    } else {
      games_query += ", " + games_placeholders.get();
    }

    games_params.append(game.site);
    games_placeholders.next();
  }
  games_query += ")";

  auto rows = tx.query<std::int64_t, std::string>(games_query, games_params);
  for (auto const &[game_id, site] : rows) {
    game_ids[site] = game_id;
  }
}

void select_board_hashes(pqxx::work &tx, const std::vector<Game> &games,
                         std::map<std::uint64_t, bool> &board_hashes) {
  std::vector<Board> boards;
  for (auto const &game : games) {
    for (const auto &continuation : game.continuations) {
      boards.push_back(continuation.board);
    }
  }

  // Batches
  const std::size_t batch_size = 65535;
  std::size_t idx = 0;
  while (idx < boards.size()) {
    std::string boards_query;
    pqxx::placeholders boards_placeholders;
    pqxx::params boards_params;

    for (std::size_t i = 0; i < batch_size && idx < boards.size(); i++) {
      const auto board = boards.at(idx++);
      if (boards_query == "") {
        boards_query = "SELECT hash from boards where hash in (" +
                       boards_placeholders.get();
      } else {
        boards_query += ", " + boards_placeholders.get();
      }

      boards_params.append(board.hash);
      boards_placeholders.next();
    }

    boards_query += ")";

    auto rows = tx.query<uint64_t>(boards_query, boards_params);
    for (auto const &[hash] : rows) {
      board_hashes[hash] = true;
    }
  }
}

void insert_boards(pqxx::work &tx, const std::vector<Game> &games,
                   std::map<std::uint64_t, bool> &seen) {
  const pqxx::table_path boards_table_path = {"boards"};

  auto boards_table = pqxx::stream_to::table(tx, boards_table_path,
                                             {
                                                 "hash",
                                                 "white_bishops",
                                                 "white_rooks",
                                                 "white_queens",
                                                 "white_knights",
                                                 "white_king",
                                                 "white_pawn",
                                                 "black_bishops",
                                                 "black_rooks",
                                                 "black_queens",
                                                 "black_knights",
                                                 "black_king",
                                                 "black_pawn",
                                                 "white_to_move",
                                                 "castle_rights",
                                                 "enpassant_sq",
                                             });

  for (const auto &game : games) {
    for (const auto &continuation : game.continuations) {
      const auto &board = continuation.board;

      if (seen.count(board.hash) > 0) {
        continue;
      }

      boards_table.write_values(
          board.hash, to_bits(board.white_bishops), to_bits(board.white_rooks),
          to_bits(board.white_queens), to_bits(board.white_knights),
          to_bits(board.white_king), to_bits(board.white_pawn),
          to_bits(board.black_bishops), to_bits(board.black_rooks),
          to_bits(board.black_queens), to_bits(board.black_knights),
          to_bits(board.black_king), to_bits(board.black_pawn),
          board.white_to_move, board.castle_rights, board.enpassant_sq);

      seen[board.hash] = true;
    }
  }

  boards_table.complete();
}

void insert_endgames(pqxx::work &tx, const std::vector<Game> &games,
                     std::map<std::string, std::int64_t> &game_ids) {
  for (auto const &game : games) {
    const auto game_id = game_ids.at(game.site);
    for (auto const &endgame : game.endgames) {
      tx.exec_prepared("insert_endgames", game_id, endgame);
    }
  }
}

void insert_continuations(pqxx::work &tx, const std::vector<Game> &games,
                          std::map<std::string, std::int64_t> &game_ids) {
  pqxx::placeholders placeholders;
  pqxx::params params;

  std::string insert;
  for (auto const &game : games) {
    const auto game_id = game_ids.at(game.site);
    for (auto const &continuation : game.continuations) {
      const auto &board = continuation.board;
      if (insert == "") {
        insert = "insert into continuations (game_id, last_board_hash, "
                 "next_board_hash, move_uci, ply) values ";
      } else {
        insert += ", ";
      }
      insert += "(" + placeholders.get() + ",";
      placeholders.next();
      params.append(game_id);

      insert += placeholders.get() + ",";
      placeholders.next();
      params.append(board.hash);

      insert += placeholders.get() + ",";
      placeholders.next();
      params.append(continuation.next_board_hash);

      insert += placeholders.get() + ",";
      placeholders.next();
      params.append(continuation.move_uci);

      insert += placeholders.get();
      placeholders.next();
      params.append(continuation.ply);

      insert += ")";
    }
  }

  tx.exec_params(insert, params);
}

void commit(pqxx::connection &conn, const std::vector<Game> &todo) {
  pqxx::work tx(conn);

  DEBUG_TIME_START(game_work);
  std::map<std::string, std::int64_t> game_ids;
  // std::cout << "Selecting existing game ids" << std::endl;
  select_game_ids(tx, todo, game_ids);
  // std::cout << "inserting new game ids" << std::endl;
  insert_games(tx, todo, game_ids);
  // std::cout << "Selecting newly inserted game ids" << std::endl;
  select_game_ids(tx, todo, game_ids);
  DEBUG_TIME_END(game_work);

  std::map<std::uint64_t, bool> board_hashes;
  DEBUG_TIME_START(board_work);
  // std::cout << "selecting boards" << std::endl;
  select_board_hashes(tx, todo, board_hashes);
  // std::cout << "inserting boards" << std::endl;
  insert_boards(tx, todo, board_hashes);
  DEBUG_TIME_END(board_work);

  DEBUG_TIME_START(continuation_work);
  // std::cout << "Inserting continuations" << std::endl;
  insert_continuations(tx, todo, game_ids);
  // std::cout << "Done!" << std::endl;
  DEBUG_TIME_END(continuation_work);

  DEBUG_TIME_START(endgame_work);
  // std::cout << "Inserting continuations" << std::endl;
  insert_endgames(tx, todo, game_ids);
  // std::cout << "Done!" << std::endl;
  DEBUG_TIME_END(endgame_work);

  DEBUG_TIME_START(commit);
  tx.commit();
  DEBUG_TIME_END(commit);
}

class MyVisitor {
private:
  chess::Board _board;
  std::map<std::string, std::string> _headers;
  std::vector<Game> _todo;
  Game _game;
  pqxx::connection &_conn;
  std::ostream &_out;

public:
  MyVisitor(pqxx::connection &conn, std::ostream &out)
      : _conn(conn), _out(out) {}

  void readGames(std::ifstream &in) {
    std::vector<std::string> row;
    while (get_row(in, row)) {
      startPgn();
      headers(row);
      const std::string &pgn = row.at(row.size() - 1);

      std::stringstream ss(pgn);
      std::string move_uci;
      while (std::getline(ss, move_uci, ' ')) {
        if (move_uci.size() <= 0) {
          continue;
        }

        char &c = move_uci.at(0);
        if (c == '1' || c == '0' || c == '*') {
          break;
        }

        move(move_uci);
      }

      endPgn();
    }

    flush();
  }

  void startPgn() {
    _board = chess::Board(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    _game = Game{};
  }

  void headers(const std::vector<std::string> &row) {
    int column = 0;
    _game.white = row.at(column++);
    _game.white_elo = row.at(column++);
    _game.white_fide_id = row.at(column++);
    _game.white_rating_diff = row.at(column++);
    _game.white_team = row.at(column++);
    _game.white_title = row.at(column++);
    _game.black = row.at(column++);
    _game.black_elo = row.at(column++);
    _game.black_fide_id = row.at(column++);
    _game.black_rating_diff = row.at(column++);
    _game.black_team = row.at(column++);
    _game.black_title = row.at(column++);
    _game.annotator = row.at(column++);
    _game.board = row.at(column++);
    _game.date = row.at(column++);
    _game.eco = row.at(column++);
    _game.event = row.at(column++);
    _game.opening = row.at(column++);
    _game.result = row.at(column++);
    _game.round = row.at(column++);
    _game.site = row.at(column++);
    _game.termination = row.at(column++);
    _game.time_control = row.at(column++);
    _game.utc_date = row.at(column++);
    _game.utc_time = row.at(column++);
  }

  void move(const std::string &uci) {
    _game.ply++;

    Board db_board = {
        .white_bishops =
            _board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE)
                .getBits(),
        .white_rooks =
            _board.pieces(chess::PieceType::ROOK, chess::Color::WHITE)
                .getBits(),
        .white_queens =
            _board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE)
                .getBits(),
        .white_knights =
            _board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE)
                .getBits(),
        .white_king = _board.pieces(chess::PieceType::KING, chess::Color::WHITE)
                          .getBits(),
        .white_pawn = _board.pieces(chess::PieceType::PAWN, chess::Color::WHITE)
                          .getBits(),
        .black_bishops =
            _board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK)
                .getBits(),
        .black_rooks =
            _board.pieces(chess::PieceType::ROOK, chess::Color::BLACK)
                .getBits(),
        .black_queens =
            _board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK)
                .getBits(),
        .black_knights =
            _board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK)
                .getBits(),
        .black_king = _board.pieces(chess::PieceType::KING, chess::Color::BLACK)
                          .getBits(),
        .black_pawn = _board.pieces(chess::PieceType::PAWN, chess::Color::BLACK)
                          .getBits(),
        .white_to_move = _board.sideToMove() == chess::Color::WHITE,
        .castle_rights = _board.getCastleString(),
        .enpassant_sq = _board.enpassantSq().index(),
        .hash = _board.hash(),
    };

    const chess::Move move = chess::uci::uciToMove(_board, uci);

    _board.makeMove(move);

    const auto next_board_hash = _board.hash();

    _game.continuations.push_back(
        Continuation{.board = db_board,
                     .move_uci = uci,
                     .next_board_hash = next_board_hash,
                     .ply = _game.ply});

    const std::string endgame = classify_endgame(_board);
    if (endgame != "") {
      _game.endgames.push_back(endgame);
    }
  }

  void endPgn() {
    _todo.push_back(_game);
    _out << _game.event << '\t' << _game.white << " (" << _game.white_elo
         << ")\t" << _game.black << " (" << _game.black_elo << ")" << "\t"
         << _game.site << std::endl;

    if (_todo.size() >= 100) {
      std::cout << "ok" << std::endl;
      flush();
    }
  }

  void flush() {
    if (_todo.empty()) {
      return;
    }

    DEBUG_TIME_START(flush);
    commit(_conn, _todo);
    _todo.clear();
    DEBUG_TIME_END(flush);
  }
};

void prepare_insert_boards(pqxx::connection &conn) {
  conn.prepare(
      "insert_boards",
      "insert into boards (hash, white_bishops, white_rooks, white_queens, "
      "white_knights, white_king, white_pawn, black_bishops, black_rooks, "
      "black_queens, black_knights, black_king, black_pawn, white_to_move, "
      "castle_rights, enpassant_sq) values ($1, $2, $3, $4, $5, $6, $7, $8, "
      "$8, $10, $11, $12, $13, $14, $15, $16) on conflict (hash) do nothing");
}

void prepare_insert_endgames(pqxx::connection &conn) {
  conn.prepare("insert_endgames",
               "insert into endgames (game_id, endgame) values ($1, $2)"
               "on conflict (game_id, endgame) do nothing");
}

int cmd_intake(std::ifstream &in, std::ostream &out) {
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

  std::cout << conn_str << std::endl;
  pqxx::connection conn(conn_str);
  if (conn.is_open()) {
    prepare_insert_endgames(conn);

    std::cout << "Connected to database successfully." << std::endl;

    auto vis = std::make_unique<MyVisitor>(conn, out);

    vis->readGames(in);

    conn.close();
  } else {
    std::cerr << "Failed to connect to database." << std::endl;

    return 1;
  }

  return 0;
}
