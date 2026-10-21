#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &lines) {
    Strings expected;
    if (args[0] == "hello") {
      require(args.size() == 1, "Invalid hello check arguments");
      expected.assign(4, "Hello World!");
    } else {
      require(args[0] == "ids" && args.size() == 3, "Invalid thread check arguments");
      const int k = std::stoi(args[1]), step = std::stoi(args[2]);
      for (int rank = 0; rank < k; rank += step)
        expected.push_back("I am " + std::to_string(rank) + " thread from " + args[1] + " threads!");
    }
    expect_lines(lines, expected);
  });
}
