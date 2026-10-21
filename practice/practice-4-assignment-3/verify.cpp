#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &lines) {
    if (args[0] == "matrix") {
      expect_numbers(lines, Strings(args.begin() + 1, args.end()));
      return;
    }
    require(args.size() >= 2, "Missing check parameters");
    const int k = std::stoi(args[1]);
    if (args[0] == "sections") {
      require(args.size() == 2, "Invalid sections check arguments");
      Strings parallel, expected;
      std::vector<int> sections;
      for (const auto &line : lines) {
        std::smatch match;
        if (std::regex_match(line, match, std::regex(R"(\[(\d+)\]: came in section (\d+))"))) {
          require(std::stoi(match[1]) < k, "Invalid section thread");
          sections.push_back(std::stoi(match[2]));
        } else {
          parallel.push_back(line);
        }
      }
      std::sort(sections.begin(), sections.end());
      require(sections == std::vector<int>({1, 2, 3}), "Missing or duplicate section");
      for (int rank = 0; rank < k; ++rank)
        expected.push_back("[" + std::to_string(rank) + "]: parallel region");
      expect_lines(parallel, expected);
      return;
    }
    require((args[0] == "sum" || args[0] == "schedule") && args.size() == 3,
            "Invalid loop check arguments");
    const bool schedule = args[0] == "schedule";
    const long long n = std::stoll(args[2]), expected = n * (n + 1) / 2;
    std::vector<int> ranks, iterations;
    std::vector<long long> sums(k), iteration_sums(k);
    long long partial = 0;
    int totals = 0;
    for (const auto &line : lines) {
      std::smatch match;
      if (std::regex_match(line, match, std::regex(R"(\[(\d+)\]: Sum = (\d+))"))) {
        const int rank = std::stoi(match[1]);
        require(rank < k, "Invalid sum thread");
        ranks.push_back(rank);
        sums[rank] = std::stoll(match[2]);
        partial += sums[rank];
      } else if (std::regex_match(line, match, std::regex(R"(Sum = (\d+))"))) {
        require(std::stoll(match[1]) == expected, "Wrong total sum");
        ++totals;
      } else if (schedule && std::regex_match(line, match,
                   std::regex(R"(\[(\d+)\]: calculation of the iteration number (\d+))"))) {
        const int rank = std::stoi(match[1]), iteration = std::stoi(match[2]);
        require(rank < k, "Invalid iteration thread");
        iterations.push_back(iteration);
        iteration_sums[rank] += iteration;
      } else {
        throw std::runtime_error("Unexpected loop output line: " + line);
      }
    }
    std::sort(ranks.begin(), ranks.end());
    require(ranks.size() == static_cast<size_t>(k), "Wrong number of partial sums");
    for (int rank = 0; rank < k; ++rank)
      require(ranks[rank] == rank, "Missing or duplicate sum thread");
    require(totals == 1 && partial == expected, "Partial sums do not match the total");
    if (schedule) {
      std::sort(iterations.begin(), iterations.end());
      require(iterations.size() == static_cast<size_t>(n), "Wrong number of iterations");
      for (int i = 0; i < n; ++i)
        require(iterations[i] == i + 1, "Missing or duplicate iteration");
      require(sums == iteration_sums, "Per-thread sum differs from assigned iterations");
    }
  });
}
