// Бабушкин Михаил Вадимович. Адаптация учебного решения Машкина А.А.
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int value = 0;
  if (size == 1) {
    std::printf("[0]: receive message '0'\n");
  } else if (rank == 0) {
    MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Recv(&value, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    std::printf("[0]: receive message '%d'\n", value);
  } else {
    MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    std::printf("[%d]: receive message '%d'\n", rank, value);
    ++value;
    MPI_Send(&value, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
