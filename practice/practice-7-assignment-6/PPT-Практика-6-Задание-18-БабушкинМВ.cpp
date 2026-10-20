#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  bool point = false, benchmark = false;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--point-to-point")
      point = true;
    else if (arg == "--benchmark")
      benchmark = true;
    else {
      std::cerr << "Флаги: --point-to-point, --benchmark.\n";
      return 2;
    }
  }
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  long long n = 0;
  if (rank == 0 && (!(std::cin >> n) || n <= 0)) {
    n = 0;
    std::cerr << "Нужно N > 0.\n";
  }
  if (point) {
    if (rank == 0)
      for (int p = 1; p < size; ++p)
        MPI_Send(&n, 1, MPI_LONG_LONG_INT, p, 0, MPI_COMM_WORLD);
    else
      MPI_Recv(&n, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
  } else
    MPI_Bcast(&n, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
  if (!n) {
    MPI_Finalize();
    return 2;
  }
  const long long base = n / size, remainder = n % size;
  const long long begin = rank * base + std::min<long long>(rank, remainder);
  const long long end = begin + base + (rank < remainder);
  const int repeats = 1;
  if (benchmark)
    MPI_Barrier(MPI_COMM_WORLD);
  const double start = MPI_Wtime();
  double local = 0, total = 0;
  const double h = 1.0 / n;
  for (long long i = begin; i < end; ++i) {
    const double x = (i + 0.5) * h;
    local += 4.0 / (1.0 + x * x);
  }
  if (point) {
    if (rank == 0) {
      total = local;
      for (int p = 1; p < size; ++p) {
        double value;
        MPI_Recv(&value, 1, MPI_DOUBLE, p, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        total += value;
      }
    } else
      MPI_Send(&local, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  } else
    MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (benchmark) {
    const double elapsed = MPI_Wtime() - start;
    double maximum;
    MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
      std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats,
                  repeats);
  }

  if (rank == 0)
    std::printf("pi = %.8f\n", total * h);

  MPI_Finalize();
}
