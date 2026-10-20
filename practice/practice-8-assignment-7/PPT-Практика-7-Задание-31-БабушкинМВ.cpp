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

  int k = 0;
  if (rank == 0 && (!(std::cin >> k) || k <= 0)) {
    k = 0;
    std::cerr << "Нужно k > 0.\n";
  }
  MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (!k) {
    MPI_Finalize();
    return 2;
  }
#pragma omp parallel num_threads(k)
  {
    std::printf(
        "I am %d thread from %d process. Number of hybrid threads = %d.\n",
        omp_get_thread_num(), rank, omp_get_num_threads() * size);
  }

  MPI_Finalize();
}
