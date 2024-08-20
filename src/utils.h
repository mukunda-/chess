#pragma once

#include <string>
#include <vector>

enum TimeControl {
  UltraBullet,
  Bullet,
  Blitz,
  Rapid,
  Classical,
  Correspondence,
  UnknownTimeControl
};

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
bool get_row(std::istream &in, std::vector<std::string> &row);
bool get_row(std::istream &in, std::vector<std::string> &row,
             std::string &line);
int get_header_index(const std::vector<std::string> &headers,
                     const std::string &header);
TimeControl get_time_control(const std::string &time_control_str);
void split(const std::string &line, std::vector<std::string> &row, char delim);
std::string get_cohort_by_lichess(int lichess_rating);
std::string get_cohort_by_lichess(const std::string &lichess_rating);
std::string classify_endgame(const std::string &material);
void print_row(std::ostream &out, const std::vector<std::string> &row);
std::string to_bits(std::uint64_t value);
