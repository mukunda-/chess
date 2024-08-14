#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "intake.h"

std::string get_option(const int argc, const char *argv[],
                       const std::string &option);
std::vector<std::string> get_positionals(const int argc, const char *argv[]);

int main(const int argc, const char *argv[]) {
  std::vector<std::string> args = get_positionals(argc, argv);
  if (args.size() < 1) {
    std::cerr << "Usage: app -o output.ctv input-a input-b" << std::endl;

    return 1;
  }
  std::string command = args[0];

  std::string out_path = get_option(argc, argv, "-o");
  if (out_path == "") {
    std::cerr << "Missing -o option to specify output file" << std::endl;
    return 1;
  }

  std::ofstream out_file(out_path);
  if (!out_file.good()) {
    std::cout << "Unable to open " << out_path << " for writing";
  }

  std::vector<std::shared_ptr<std::ifstream>> in_files;
  std::vector<std::string> in_paths(args.begin() + 1, args.end());
  for (const auto &in_path : in_paths) {
    auto in_file = std::make_shared<std::ifstream>(in_path);
    if (!in_file->good()) {
      std::cout << "Unable to open " << in_path << " for reading";
    }

    in_files.push_back(in_file);
  }

  for (auto &in_file : in_files) {
    if (command == "intake") {
      cmd_intake(*in_file, out_file);
    } else {
      std::cerr << "Command not found." << std::endl;
      return 1;
    }

    in_file->close();
  }

  out_file.close();

  return 0;
}

std::vector<std::string> get_positionals(const int argc, const char *argv[]) {
  std::vector<std::string> positionals;

  bool skip_next = false;
  for (int i = 1; i < argc; i++) {
    const std::string arg(argv[i]);

    if (skip_next) {
      skip_next = false;
      continue;
    }

    if (arg.size() > 0 && arg.at(0) == '-') {
      skip_next = true;
      continue;
    }

    positionals.push_back(arg);
  }

  return positionals;
}

std::string get_option(const int argc, const char *argv[],
                       const std::string &option) {
  bool value_next = false;
  for (int i = 1; i < argc; i++) {
    const std::string arg(argv[i]);

    if (value_next) {
      return arg;
    }

    if (arg == option) {
      value_next = true;
    }
  }

  return "";
}
