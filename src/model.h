#pragma once

#include "chess.hpp"

struct EndgameClassComp {
  chess::PieceType piece_type;
  chess::Color color;
  char out;
};

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
