#include <cstdio>
#include <iostream>
#include <omp.h>

int main() {
  int k;
  if (!(std::cin >> k) || k <= 0) {
    std::cerr << "Нужно k > 0.\n";
    return 2;
  }
  omp_set_dynamic(0);
#pragma omp parallel num_threads(k)
  {
#pragma omp sections
    {
#pragma omp section
      {
        std::printf("[%d]: came in section 1\n", omp_get_thread_num());
      }
#pragma omp section
      {
        std::printf("[%d]: came in section 2\n", omp_get_thread_num());
      }
#pragma omp section
      {
        std::printf("[%d]: came in section 3\n", omp_get_thread_num());
      }
    }
    std::printf("[%d]: parallel region\n", omp_get_thread_num());
  }
}
