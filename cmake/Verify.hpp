#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Strings = std::vector<std::string>;

inline void require(bool condition, const std::string &message) {
  if (!condition)
    throw std::runtime_error(message);
}

inline void expect_lines(Strings actual, Strings expected) {
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  require(actual == expected, "Output lines differ from the expected multiset");
}

inline void expect_numbers(const Strings &lines, const Strings &expected) {
  std::vector<double> actual;
  for (const auto &line : lines) {
    std::istringstream input(line);
    std::string token;
    while (input >> token) {
      size_t consumed = 0;
      const double value = std::stod(token, &consumed);
      require(consumed == token.size() && std::isfinite(value), "Invalid matrix value: " + token);
      actual.push_back(value);
    }
    require(input.eof(), "Unexpected text in matrix output");
  }
  require(actual.size() == expected.size(), "Wrong number of matrix elements");
  for (size_t i = 0; i < actual.size(); ++i)
    require(actual[i] == std::stod(expected[i]), "Wrong matrix element at " + std::to_string(i));
}

inline void expect_pi(const Strings &lines, double expected) {
  require(lines.size() == 1, "Expected exactly one pi result");
  std::smatch match;
  require(std::regex_match(lines[0], match, std::regex(R"(pi = ([0-9]+\.[0-9]+))")),
          "Invalid pi result format");
  const double value = std::stod(match[1]);
  require(std::isfinite(value) && std::abs(value - expected) < 1e-8, "Wrong pi result");
}

inline Strings remove_timing(const Strings &lines, int repetitions) {
  Strings result;
  int times = 0, repeats = 0;
  const std::regex time(R"(Time = ([0-9]+\.[0-9]+))");
  for (const auto &line : lines) {
    std::smatch match;
    if (std::regex_match(line, match, time)) {
      const double seconds = std::stod(match[1]);
      require(std::isfinite(seconds) && seconds > 0, "Invalid elapsed time");
      ++times;
    } else if (repetitions && line == "Repetitions = " + std::to_string(repetitions)) {
      ++repeats;
    } else {
      result.push_back(line);
    }
  }
  require(times == 1 && repeats == (repetitions ? 1 : 0), "Missing or duplicate timing metadata");
  return result;
}

template <typename Check>
int verify_main(int argc, char **argv, Check check) {
  try {
    const Strings args(argv + 1, argv + argc);
    require(!args.empty(), "Missing check name");
    Strings lines;
    std::string line;
    while (std::getline(std::cin, line)) {
      if (!line.empty() && line.back() == '\r')
        line.pop_back();
      lines.push_back(line);
    }
    require(!std::cin.bad(), "Cannot read program output");
    if (args[0] == "error") {
      require(args.size() == 2, "Missing expected error message");
      require(std::any_of(lines.begin(), lines.end(), [&](const auto &text) {
        return text.find(args[1]) != std::string::npos;
      }), "Expected input error message was not found");
    } else {
      check(args, lines);
    }
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
