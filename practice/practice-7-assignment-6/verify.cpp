#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &output) {
    std::string kind = args[0];
    Strings lines = output;
    if (kind.rfind("timed-", 0) == 0) {
      kind.erase(0, 6);
      lines = remove_timing(lines, kind == "pi" ? 1 : 1000);
    }
    if (kind == "letters") {
      require(args.size() == 2, "Missing expected word");
      Strings expected;
      for (char letter = 'a'; letter <= 'z'; ++letter) {
        const auto count = std::count(args[1].begin(), args[1].end(), letter);
        if (count)
          expected.push_back(std::string(1, letter) + " = " + std::to_string(count));
      }
      expect_lines(lines, expected);
    } else if (kind == "pi") {
      require(args.size() == 2, "Missing pi input size");
      expect_pi(lines, args[1] == "1" ? 3.2 : std::acos(-1.0));
    } else {
      require(kind == "matrix", "Unknown calculation check");
      expect_numbers(lines, Strings(args.begin() + 1, args.end()));
    }
  });
}
