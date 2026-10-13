// Бабушкин Михаил Вадимович. Адаптация учебного решения Машкина А.А.
#include <cstdio>
#include <omp.h>

int main() {
  omp_set_dynamic(0);
  omp_set_num_threads(8);
// num_threads имеет приоритет над omp_set_num_threads.
#pragma omp parallel num_threads(4)
  {
    std::printf("Hello World!\n");
  }
}
