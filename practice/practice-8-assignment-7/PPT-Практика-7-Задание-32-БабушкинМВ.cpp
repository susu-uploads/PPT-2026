#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>

int main(int argc, char **argv) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if (provided < MPI_THREAD_FUNNELED) {
    std::cerr << "MPI не поддерживает MPI_THREAD_FUNNELED.\n";
    MPI_Abort(MPI_COMM_WORLD, 2);
  }
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  omp_set_dynamic(0);

  std::string mode = "hybrid";
  if (argc == 3 && std::string(argv[1]) == "--mode")
    mode = argv[2];
  if ((argc != 1 && argc != 3) ||
      (argc == 3 && std::string(argv[1]) != "--mode") ||
      (mode != "hybrid" && mode != "serial" && mode != "openmp") ||
      (mode != "hybrid" && size != 1)) {
    if (rank == 0)
      std::cerr << "--mode serial|openmp|hybrid; serial/openmp требуют один "
                   "процесс.\n";
    MPI_Finalize();
    return 2;
  }
  long long n = 0;
  if (rank == 0 && (!(std::cin >> n) || n <= 0)) {
    n = 0;
    std::cerr << "Нужно N > 0.\n";
  }
  MPI_Bcast(&n, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
  if (!n) {
    MPI_Finalize();
    return 2;
  }
  const long long base = n / size, remainder = n % size;
  const long long begin = rank * base + std::min<long long>(rank, remainder);
  const long long end = begin + base + (rank < remainder);
  const double h = 1.0 / n;
  double local = 0, total = 0;
  MPI_Barrier(MPI_COMM_WORLD);
  const double start = MPI_Wtime();
  if (mode == "serial") {
    for (long long i = begin; i < end; ++i) {
      const double x = (i + 0.5) * h;
      local += 4.0 / (1.0 + x * x);
    }
  } else {
#pragma omp parallel for reduction(+ : local) schedule(static)
    for (long long i = begin; i < end; ++i) {
      const double x = (i + 0.5) * h;
      local += 4.0 / (1.0 + x * x);
    }
  }
  MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  const double elapsed = MPI_Wtime() - start;
  double maximum;
  MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    std::printf("pi = %.8f\nTime = %.12f\n", total * h, maximum);
    std::printf("MPI processes = %d, OpenMP threads per process = %d\n", size,
                mode == "serial" ? 1 : omp_get_max_threads());
  }

  MPI_Finalize();
}
