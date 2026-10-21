#include "Verify.hpp"

int main(int argc, char **argv) {
  return verify_main(argc, argv, [](const Strings &args, const Strings &lines) {
    Strings expected;
    require(args.size() >= 2, "Missing check parameters");
    const int k = std::stoi(args[1]);
    if (args[0] == "private") {
      require(args.size() == 2, "Invalid private check arguments");
      expected.push_back("Private:");
      for (int rank = 0; rank < k; ++rank)
        expected.push_back("I am " + std::to_string(rank) + " thread.");
    } else {
      require(args[0] == "sum" && args.size() == 4, "Invalid sum check arguments");
      const long long n = std::stoll(args[2]);
      const bool extra = args[3] == "extra";
      for (int rank = 0; rank < k; ++rank) {
        const long long count = extra ? n / k + (rank < n % k) : (rank ? n - n / 2 : n / 2);
        const long long start = extra ? rank * (n / k) + std::min<long long>(rank, n % k) + 1
                                     : (rank ? n / 2 + 1 : 1);
        const long long sum = count * (2 * start + count - 1) / 2;
        expected.push_back("[" + std::to_string(rank) + "]: Sum = " + std::to_string(sum));
      }
      expected.push_back("Sum = " + std::to_string(n * (n + 1) / 2));
    }
    expect_lines(lines, expected);
  });
}
