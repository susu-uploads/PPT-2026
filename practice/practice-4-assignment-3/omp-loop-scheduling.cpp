#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int main(int argc, char **argv) {
  int k = 4, chunk = 0;
  long long n = 10;
  omp_sched_t kind = omp_sched_static;
  const bool schedule = argc == 3 && std::string(argv[1]) == "--schedule";
  if (schedule) {
    const std::string value = argv[2];
    if (value == "static")
      kind = omp_sched_static;
    else if (value == "static,1") {
      kind = omp_sched_static;
      chunk = 1;
    } else if (value == "static,2") {
      kind = omp_sched_static;
      chunk = 2;
    } else if (value == "dynamic") {
      kind = omp_sched_dynamic;
      chunk = 1;
    } else if (value == "dynamic,2") {
      kind = omp_sched_dynamic;
      chunk = 2;
    } else if (value == "guided") {
      kind = omp_sched_guided;
      chunk = 1;
    } else if (value == "guided,2") {
      kind = omp_sched_guided;
      chunk = 2;
    } else {
      std::cerr << "Неизвестное расписание.\n";
      return 2;
    }
  } else if (argc != 1 || !(std::cin >> k >> n) || k <= 0 || n < 0 ||
             n > 4294967295LL) {
    std::cerr << "Ожидается k N; для задания 7: --schedule <расписание>.\n";
    return 2;
  }
  omp_set_dynamic(0);
  omp_set_schedule(kind, chunk);
  long long sum = 0;
#pragma omp parallel num_threads(k) reduction(+ : sum)
  {
    const int rank = omp_get_thread_num();
#pragma omp for schedule(runtime)
    for (long long i = 1; i <= n; ++i) {
      if (schedule)
        std::printf("[%d]: calculation of the iteration number %lld\n", rank,
                    i);
      sum += i;
    }
    std::printf("[%d]: Sum = %lld\n", rank, sum);
  }
  std::printf("Sum = %lld\n", sum);
}
