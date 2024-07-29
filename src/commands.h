#pragma once

#include <iostream>
#include <vector>
#include <memory>

#define WIN "win"
#define LOSS "loss"
#define DRAW "1/2-1/2"
#define WHITE_WINS "1-0"
#define BLACK_WINS "0-1"
#define ALL "all"

int cmd_prepend_cohorts(std::ostream& out);
int cmd_first_n_rows(std::ostream& out);
int cmd_last_line(std::ostream& out);
int cmd_count_time_controls(std::ostream& out);
int cmd_stats(std::istream& in, std::ostream& out);
int cmd_endgame_by_cohort(std::istream &in, std::ostream &out);
int cmd_group_by_two(std::istream &in, std::ostream &out);
int cmd_classify_endgames(std::vector<std::shared_ptr<std::ifstream>> in_files, std::ostream &out);
