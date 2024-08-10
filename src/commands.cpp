#include "commands.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

/*
const std::vector<std::string> header_names = {
    "White",       "WhiteElo",
    "WhiteFideId", "WhiteRatingDiff",
    "WhiteTeam",   "WhiteTitle",
    "Black",       "BlackElo",
    "BlackFideId", "BlackRatingDiff",
    "BlackTeam",   "BlackTitle",
    "Annotator",   "Board",
    "Date",        "ECO",
    "Event",       "Opening",
    "Result",      "Round",
    "Site",        "Termination",
    "TimeControl", "UTCDate",
    "UTCTime",     "PGN",
};
*/

void print_col_indexes(std::ostream &out, std::vector<std::string> row) {
  for (std::size_t idx = 0; idx < row.size(); idx++) {
    out << idx << " " << row[idx] << std::endl;
  }
}

/*
int cmd_parse_pgns(std::istream &in, std::ostream &out) {
  const std::vector<std::string> header_names = {
    "White",
    "WhiteElo",
    "WhiteFideId",
    "WhiteRatingDiff",
    "WhiteTeam",
    "WhiteTitle",
    "Black",
    "BlackElo",
    "BlackFideId",
    "BlackRatingDiff",
    "BlackTeam",
    "BlackTitle",
    "Annotator",
    "Board",
    "Date",
    "ECO",
    "Event",
    "Opening",
    "Result",
    "Round",
    "Site",
    "Termination",
    "TimeControl",
    "UTCDate",
    "UTCTime",
    "PGN",
  }

  int pgn_idx = get_header_index(header_names);
  std::cout << pgn_idx << std::endl;
}
*/

int cmd_stats(std::istream &in, std::ostream &out) {
  std::size_t count = 0;

  std::vector<std::string> row;
  while (get_row(in, row)) {
    count += 1;
  }

  out << count << std::endl;

  return 0;
}

int cmd_group_by_two(std::istream &in, std::ostream &out) {
  std::map<std::string, std::map<std::string, int>> counts_by_first_feature;

  const std::size_t count_idx = 0;
  const std::size_t first_feature_idx = 1;
  const std::size_t second_feature_idx = 2;

  std::vector<std::string> row;
  while (get_row(in, row)) {
    if (first_feature_idx >= row.size()) {
      std::cerr << "row too small for first_feature" << std::endl;
      return 1;
    }

    if (second_feature_idx >= row.size()) {
      std::cerr << "row too small for second feature" << std::endl;
      return 1;
    }

    if (count_idx >= row.size()) {
      std::cerr << "row too small for count" << std::endl;
      return 1;
    }

    const std::string first_feature = row.at(first_feature_idx);
    const std::string second_feature = row.at(second_feature_idx);
    counts_by_first_feature[first_feature][second_feature] += std::stoi(row.at(count_idx));
  }

  for (const auto &[first_feature, counts] : counts_by_first_feature) {
    for (const auto &[second_feature, count] : counts) {
      out << count << "\t" << first_feature << "\t" << second_feature << "\t" <<  std::endl;
    }
  }

  return 0;
}

/*
int cmd_classify_endgames(std::vector<std::shared_ptr<std::ifstream>> in_files, std::ostream &out) {
  const int fen_idx = 0;

  std::vector<std::string> row;
  std::string line;
  for (auto &in : in_files) { 
    while (get_row(*in, row)) {
      const std::string& fen = row.at(fen_idx);
      const std::string endgame = classify_endgame(fen);
      if (endgame.empty()) {
        continue;
      }

      row.push_back(endgame);
      print_row(out, row);
    }
  }

  return 0;
}
*/


int cmd_endgame_by_cohort(std::istream &in, std::ostream &out) {
  const int mat_idx = 0;
  const int white_elo_idx = 3;
  const int time_control_idx = 5;
  const int result_idx = 9;

  std::map<std::string, std::map<std::string, std::map<std::string, int>>> results_by_cohot;
  std::map<std::string, std::map<std::string, int>> cohort_totals;

  std::vector<std::string> row;

  while (get_row(in, row)) {
    if (white_elo_idx >= row.size()) {
      std::cerr << "row too small for white elo idx" << std::endl;
      return 1;
    }
    const std::string white_cohort = get_cohort_by_lichess(row[white_elo_idx]);

    if (time_control_idx >= row.size()) {
      std::cerr << "row too small for time control idx" << std::endl;
      return 1;
    }

    if (result_idx >= row.size()) {
      std::cerr << "row too small for result idx" << std::endl;
      return 1;
    }

    const TimeControl time_control = get_time_control(row.at(time_control_idx));
    if (time_control != Rapid && time_control != Classical) {
      continue;
    }

    const std::string endgame = classify_endgame(row.at(mat_idx));
    if (endgame.empty()) {
      continue;
    }

    const std::string result = row[result_idx];
    if (result == DRAW) {
      results_by_cohot[white_cohort][endgame][DRAW] += 1;
    } else if (result == WHITE_WINS) {
      results_by_cohot[white_cohort][endgame][WIN] += 1;
    } else if (result == BLACK_WINS) {
      results_by_cohot[white_cohort][endgame][LOSS] += 1;
    }

    cohort_totals[white_cohort][endgame] += 1;
  }

  for (const auto &[cohort, endgame_counts] : results_by_cohot) {
    for (const auto &[endgame, result_counts] : endgame_counts) {
      for (const auto &[result, count] : result_counts) {
        out << count << "\t" << cohort << "\t" << endgame << "\t" << result << std::endl;
      }
    }
  }


  for (const auto &[cohort, endgame_counts] : cohort_totals) {
    for (const auto &[endgame, count] : endgame_counts) {
      out << count << "\t" << cohort << "\t" << endgame << "\t" << ALL << std::endl;
    }
  }

  return 0;
}

int cmd_prepend_cohorts(std::ostream &out) {
  std::string header_line =
      "game_id	result	eco	site	time_control	white_username	"
      "white_elo	black_username	black_elo	"
      "white_rating_diff	black_rating_diff	event	fen	"
      "ply	P	N	B	R	Q	K	p	"
      "n	b	r	q	k";
  std::vector<std::string> headers;
  split(header_line, headers, '\t');

  const int white_elo_idx = get_header_index(headers, "white_elo");
  const int black_elo_idx = get_header_index(headers, "black_elo");

  std::vector<std::string> row;
  while (get_row(std::cin, row)) {
    const std::string white_cohort = get_cohort_by_lichess(row[white_elo_idx]);
    const std::string black_cohort = get_cohort_by_lichess(row[black_elo_idx]);

    out << row[white_elo_idx] << " " << white_cohort << " "
        << row[black_elo_idx] << " " << black_cohort << std::endl;
  }

  return 0;
}

int cmd_first_n_rows(std::ostream &out) {
  std::string header_line = "cohort\tcount\ttime_control";
  std::vector<std::string> headers;
  split(header_line, headers, '\t');

  const int comparison_idx = 0;
  std::string last_value;
  const int target_print_count = 3;
  int prints_left = target_print_count;

  std::vector<std::string> row;
  std::string line;
  while (get_row(std::cin, row, line)) {
    const std::string value = row[comparison_idx];

    if (value != last_value) {
      prints_left = 3;
    }

    if (prints_left > 0) {
      out << line << std::endl;
      prints_left -= 1;
    }

    last_value = value;
  }

  return 0;
}

int cmd_last_line(std::ostream &out) {
  std::string header_line = "cohort\tcount\ttime_control";
  std::vector<std::string> headers;
  split(header_line, headers, '\t');

  int comparison_idx = 0;
  std::string last_value;
  std::string last_line;
  std::vector<std::string> row;
  std::string line;
  while (get_row(std::cin, row, line)) {
    const std::string value = row[comparison_idx];
    if (!last_line.empty() && value != last_value) {
      out << last_line << std::endl;
    }

    last_line = line;
    last_value = value;
  };

  out << last_line << std::endl;

  return 0;
}

int cmd_count_time_controls(std::ostream &out) {
  std::string header_line =
      "game_id	result	eco	site	time_control	white_username	"
      "white_elo	black_username	black_elo	"
      "white_rating_diff	black_rating_diff	event	fen	"
      "ply	P	N	B	R	Q	K	p	"
      "n	b	r	q	k";
  std::vector<std::string> headers;
  split(header_line, headers, '\t');

  const int white_elo_idx = get_header_index(headers, "white_elo");
  const int black_elo_idx = get_header_index(headers, "black_elo");
  const int time_ctrl_idx = get_header_index(headers, "time_control");

  std::map<std::string, std::map<std::string, int>> time_controls_per_cohort;

  std::vector<std::string> row;
  while (get_row(std::cin, row)) {
    const std::string white_cohort = get_cohort_by_lichess(row[white_elo_idx]);
    const std::string black_cohort = get_cohort_by_lichess(row[black_elo_idx]);
    const std::string time_control = row[time_ctrl_idx];

    for (const auto &cohort : {white_cohort, black_cohort}) {
      time_controls_per_cohort[cohort][time_control] += 1;
    }
  }

  for (const auto &[cohort, time_controls] : time_controls_per_cohort) {
    for (const auto &[time_control, count] : time_controls) {
      out << cohort << "\t" << count << "\t" << time_control << std::endl;
    }
  }

  return 0;
}
