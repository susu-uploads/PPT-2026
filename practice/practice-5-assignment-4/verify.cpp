#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &lines) {
    require(args.size() == 2, "Invalid MPI check arguments");
    const int size = std::stoi(args[1]);
    Strings expected;
    if (args[0] == "hello") {
      for (int rank = 0; rank < size; ++rank)
        expected.push_back("I am " + std::to_string(rank) + " process from " + args[1] + " processes!");
    } else {
      require(args[0] == "parity", "Unknown MPI check");
      expected.push_back(args[1] + " processes.");
      for (int rank = 1; rank < size; ++rank)
        expected.push_back("I am " + std::to_string(rank) + ": " + (rank % 2 ? "SECOND!" : "FIRST!"));
    }
    expect_lines(lines, expected);
  });
}
