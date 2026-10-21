#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (int offset = 1; offset < size; ++offset) {
    const int destination = (rank + offset) % size;
    const int source = (rank - offset + size) % size;
    int value = -1;
    MPI_Sendrecv(&rank, 1, MPI_INT, destination, 0, &value, 1, MPI_INT, source,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::printf("[%d]: receive message '%d' from %d\n", rank, value, source);
  }

  MPI_Finalize();
}
