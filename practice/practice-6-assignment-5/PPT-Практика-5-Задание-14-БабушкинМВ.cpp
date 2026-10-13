// Бабушкин Михаил Вадимович. Адаптация учебного решения Машкина А.А.
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    for (int source = 1; source < size; ++source) {
      int value;
      MPI_Recv(&value, 1, MPI_INT, source, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      std::printf("receive message '%d' from %d\n", value, source);
    }
  } else
    MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  MPI_Finalize();
}
