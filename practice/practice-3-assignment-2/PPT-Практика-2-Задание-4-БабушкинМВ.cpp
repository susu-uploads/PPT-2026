#include <chrono>
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <thread>

int main(int argc, char **argv) {
  std::string mode = "all";
  if (argc == 3 && std::string(argv[1]) == "--mode")
    mode = argv[2];
  if ((argc != 1 && argc != 3) ||
      (argc == 3 && std::string(argv[1]) != "--mode") ||
      (mode != "all" && mode != "shared" && mode != "shared-sleep" &&
       mode != "private")) {
    std::cerr << "--mode all|shared|shared-sleep|private\n";
    return 2;
  }
  int k;
  if (!(std::cin >> k) || k <= 0) {
    std::cerr << "Нужно k > 0.\n";
    return 2;
  }
  omp_set_dynamic(0);
  int rank = 0;
  if (mode == "all" || mode == "shared") {
    std::puts("Shared:");
#pragma omp parallel num_threads(k) shared(rank)
    {
      rank = omp_get_thread_num();
      std::printf("I am %d thread.\n", rank);
    }
  }
  if (mode == "all" || mode == "shared-sleep") {
    std::puts("Shared with sleep:");
#pragma omp parallel num_threads(k) shared(rank)
    {
      rank = omp_get_thread_num();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::printf("I am %d thread.\n", rank);
    }
  }
  if (mode == "all" || mode == "private") {
    std::puts("Private:");
#pragma omp parallel num_threads(k) private(rank)
    {
      rank = omp_get_thread_num();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::printf("I am %d thread.\n", rank);
    }
  }
}
