// Бабушкин Михаил Вадимович. Адаптация учебного решения Машкина А.А.
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int main(int argc, char **argv) {
  const bool even = argc == 2 && std::string(argv[1]) == "--even";
  int k;
  if ((argc != 1 && !even) || !(std::cin >> k) || k <= 0) {
    std::cerr << "Введите положительное число потоков; вариант: --even.\n";
    return 2;
  }
  omp_set_dynamic(0);
#pragma omp parallel num_threads(k)
  {
    const int rank = omp_get_thread_num();
    if (!even || rank % 2 == 0)
      std::printf("I am %d thread from %d threads!\n", rank,
                  omp_get_num_threads());
  }
}
