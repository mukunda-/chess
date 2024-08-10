#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <map>

#include "endgames.h"

#include "chess.hpp"

#define SEP '\t'

#include "debug.h"

#define LIGHT_SQUARES_MASK 0x55AA55AA55AA55AA
#define DARK_SQUARES_MASK 0xAA55AA55AA55AA55

struct EndgameClassComp {
  chess::PieceType piece_type;
  chess::Color color;
  char out;
};

std::string classify_endgame(const chess::Board& board) {
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
    {chess::PieceType::PAWN, chess::Color::BLACK, 'P'}
  };
  constexpr std::array<uint64_t, 2> square_masks = {LIGHT_SQUARES_MASK, DARK_SQUARES_MASK};

  std::string out;
  int total_pieces = 0;
  for (const auto& comp : endgameComps) {
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
  for (const chess::Color &color : {chess::Color::WHITE, chess::Color::BLACK}) {
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

class MyVisitor : public chess::pgn::Visitor {
private:
  std::ostream &_out;
  chess::Board _board;
  std::map<std::string, bool> _seen_endgames;
  std::stringstream _moves;
  std::stringstream _headers;
  std::stringstream _endgames;
  std::string _site;
  bool _empty;

public:
  MyVisitor(std::ostream &out) : _out(out), _empty(true) {
  }

  void header(std::string_view key, std::string_view value) {
    _headers << SEP << key << SEP << value;

    if (key == "Site") {
      _site = value;
    }
  }

  void move(std::string_view san, std::string_view comment) {
    _empty = false;

    const chess::Move move = chess::uci::parseSan(_board, san);

    _board.makeMove(move);

    const std::string endgame = classify_endgame(_board);
    if (endgame != "" && !_seen_endgames[endgame]) {
      _seen_endgames[endgame] = true;
      _endgames << SEP << endgame;
    }

    _moves << SEP << chess::uci::moveToUci(move);
  }

  void startPgn() {
    _board = chess::Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    _seen_endgames.clear();
    _moves.str(std::string());
    _endgames.str(std::string());
    _headers.str(std::string());
    _site = "";
    _empty = true;
  }

  void startMoves() {}

  void endPgn() {
    if (_empty) {
      return;
    }

    _out << "set_headers" << SEP << _site << _headers.str() << std::endl;
    _out << "set_moves" << SEP << _site << _moves.str() << std::endl;
    if (!_seen_endgames.empty()) {
      _out << "set_endgames" << SEP << _site << _endgames.str() << std::endl;
    }

    std::cerr << "ok" << std::endl;
  }
};

int cmd_classify_endgames(std::ifstream &in, std::ostream &out) {
  auto vis = std::make_unique<MyVisitor>(out);

  chess::pgn::StreamParser parser(in);
  parser.readGames(*vis);

  return 0;
}
