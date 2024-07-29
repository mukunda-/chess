#include "utils.h"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <ranges>

// Define the rating boundaries with Lichess values as keys
std::map<int, std::string> rating_boundaries = {
    {1035, "0-300"},     {1100, "300-400"},   {1165, "400-500"},
    {1225, "500-600"},   {1290, "600-700"},   {1350, "700-800"},
    {1415, "800-900"},   {1475, "900-1000"},  {1575, "1000-1100"},
    {1675, "1100-1200"}, {1750, "1200-1300"}, {1825, "1300-1400"},
    {1900, "1400-1500"}, {2000, "1500-1600"}, {2075, "1600-1700"},
    {2150, "1700-1800"}, {2225, "1800-1900"}, {2300, "1900-2000"},
    {2375, "2000-2100"}, {2450, "2100-2200"}, {2525, "2200-2300"},
    {2600, "2300-2400"}};

// Trim from start (in place)
void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// Trim from end (in place)
void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// Trim from both ends (in place)
void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

bool get_row(std::istream& in, std::vector<std::string>& row) {
  std::string line;

  return get_row(in, row, line);
}

bool get_row(std::istream& in, std::vector<std::string>& row, std::string& line) {
  if (!std::getline(in, line)) {
    return false;
  }


  // Split the line by tab character
  split(line, row, '\t');

  return true;
}

int get_header_index(const std::vector<std::string> &headers,
                     const std::string &header) {
  for (std::size_t i = 0; i < headers.size(); i++) {
    if (headers[i] == header) {
      return i;
    }
  }

  return -1;
}


void split(const std::string &line, std::vector<std::string> &row, char delim) {
  // Split the line by tab character
  std::stringstream ss(line);
  std::string col;
  row.clear();
  while (std::getline(ss, col, delim)) {
    row.push_back(col);
  }
}

int parse_rating(const std::string &rating) { return std::stoi(rating); }

// Function to get the dojo cohort based on Lichess rating
std::string get_cohort_by_lichess(int lichess_rating) {

  // Find the cohort based on the Lichess rating
  for (const auto& [rating_floor, cohort] : rating_boundaries) {
    if (lichess_rating < rating_floor) {
      return cohort;
    }
  }

  return "2400p";
}

std::string get_cohort_by_lichess(const std::string& lichess_rating) {
  return get_cohort_by_lichess(parse_rating(lichess_rating));
}

// https://lichess.org/faq#time-controls
TimeControl get_time_control(const std::string& time_control_str) {
  if (time_control_str == "-") {
    return Correspondence;
  }
  std::vector<std::string> components;
  split(time_control_str, components, '+');
  
  if (components.size() != 2) {
    throw std::invalid_argument("received invalid time control");
  }

  int initial_s = std::stoi(components.at(0));
  int increment_s = std::stoi(components.at(1));

  int game_time = initial_s + (increment_s * 40);

  if (game_time <= 29) {
    return UltraBullet;
  }

  if (game_time <= 179) {
    return Bullet;
  }

  if (game_time <= 479) {
    return Blitz;
  }

  if (game_time <= 1499) {
    return Rapid;
  }

  if (game_time >= 1500) {
    return Classical;
  }

  return UnknownTimeControl;
}

void transform_upper(std::string& str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string classify_endgame(const std::string& fen) {
  std::map<char, int> counts;

  for (const char& c : fen) {
    if (c == ' ') {
      break;
    }

    counts[c] += 1;
  }

  std::vector<char> pieces = {'K', 'B', 'N', 'R', 'Q', 'P', 'k', 'b', 'n', 'r', 'q', 'p'};
  int total_pieces = 0;
  std::ostringstream os;
  for (auto& piece : pieces) {
    int count = counts[piece];
    std::string piece_upper(piece, 1);
    transform_upper(piece_upper);

    for (int j = 0; j < count; j++) {
      os << piece;
    }

    total_pieces += count;
    if (total_pieces > 5) {
      return "";
    }
  }

  std::string result = os.str();
  transform_upper(result);

  return result;
}

void print_row(std::ostream& out, const std::vector<std::string> &row) {
  if (row.empty()) {
    return;
  }

  for (size_t i = 0; i < row.size() - 1; i++) {
    out << row.at(i) << "\t";
  }

  out << row.back() << "\n";
}
