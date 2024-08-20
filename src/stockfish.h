#pragma once

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#include <boost/asio.hpp>
#include <boost/process.hpp>

namespace stockfish {
void send_new_game(boost::process::opstream &os);

void send_eval(boost::process::opstream &os, const std::string &moves);

void send_quit(boost::process::opstream &os);

void send_options(boost::process::opstream &os);

void read_response(boost::process::opstream &os, boost::process::ipstream &is);

bool read_score_cp(boost::process::opstream &os, boost::process::ipstream &is,
                   int *eval);
} // namespace stockfish
