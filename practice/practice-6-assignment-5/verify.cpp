#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &lines) {
    require(args.size() == 2, "Invalid exchange check arguments");
    const int size = std::stoi(args[1]);
    Strings expected;
    for (int rank = 0; rank < size; ++rank) {
      const std::string prefix = "[" + std::to_string(rank) + "]: receive message '";
      if (args[0] == "relay") {
        expected.push_back(prefix + std::to_string(rank ? rank - 1 : size - 1) + "'");
      } else if (args[0] == "master") {
        if (rank)
          expected.push_back("receive message '" + std::to_string(rank) + "' from " + std::to_string(rank));
      } else if (args[0] == "ring") {
        expected.push_back(prefix + std::to_string((rank + size - 1) % size) + "'");
      } else {
        require(args[0] == "all-to-all", "Unknown exchange check");
        for (int source = 0; source < size; ++source)
          if (source != rank)
            expected.push_back(prefix + std::to_string(source) + "' from " + std::to_string(source));
      }
    }
    expect_lines(lines, expected);
  });
}
