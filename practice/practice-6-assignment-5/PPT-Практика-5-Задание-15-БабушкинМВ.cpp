// Бабушкин Михаил Вадимович. Адаптация учебного решения Машкина А.А.
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int value = -1;
  const int next = (rank + 1) % size, previous = (rank - 1 + size) % size;
  // Блокирующий парный обмен не зависит от буферизации MPI_Send.
  MPI_Sendrecv(&rank, 1, MPI_INT, next, 0, &value, 1, MPI_INT, previous, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  std::printf("[%d]: receive message '%d'\n", rank, value);

  MPI_Finalize();
}
