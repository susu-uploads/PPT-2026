#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int main(int argc, char **argv) {
  const bool extra = argc == 2 && std::string(argv[1]) == "--extra";
  int k = 2;
  long long n;
  if ((argc != 1 && !extra) || (extra && !(std::cin >> k)) ||
      !(std::cin >> n) || k <= 0 || n < 0 || n > 4294967295LL) {
    std::cerr << "Ожидается N (0..4294967295), для --extra: k N, k > 0.\n";
    return 2;
  }
  omp_set_dynamic(0);
  long long sum = 0;
#pragma omp parallel num_threads(k) reduction(+ : sum)
  {
    const int rank = omp_get_thread_num(), size = omp_get_num_threads();
    long long start, end;
    if (!extra && rank == 0) {
      start = 1;
      end = n / 2;
    } else if (!extra) {
      start = n / 2 + 1;
      end = n;
    } else {
      const long long base = n / size, remainder = n % size;
      start = rank * base + (rank < remainder ? rank : remainder) + 1;
      end = start + base + (rank < remainder) - 1;
    }
    for (long long i = start; i <= end; ++i)
      sum += i;
    std::printf("[%d]: Sum = %lld\n", rank, sum);
  }
  std::printf("Sum = %lld\n", sum);
}
