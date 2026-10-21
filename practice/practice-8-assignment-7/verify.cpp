#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &output) {
    require(args.size() >= 3, "Missing hybrid check parameters");
    const int size = std::stoi(args[1]), threads = std::stoi(args[2]);
    if (args[0] == "hello") {
      require(args.size() == 3, "Invalid hybrid hello check arguments");
      Strings expected;
      for (int rank = 0; rank < size; ++rank)
        for (int thread = 0; thread < threads; ++thread)
          expected.push_back("I am " + std::to_string(thread) + " thread from " +
              std::to_string(rank) + " process. Number of hybrid threads = " +
              std::to_string(size * threads) + ".");
      expect_lines(output, expected);
    } else {
      require(args[0] == "pi" && args.size() == 4, "Invalid hybrid pi check arguments");
      Strings lines = remove_timing(output, 0);
      const std::string summary = "MPI processes = " + args[1] +
          ", OpenMP threads per process = " + args[2];
      require(std::count(lines.begin(), lines.end(), summary) == 1, "Wrong hybrid process/thread counts");
      lines.erase(std::find(lines.begin(), lines.end(), summary));
      expect_pi(lines, args[3] == "1" ? 3.2 : std::acos(-1.0));
    }
  });
}
