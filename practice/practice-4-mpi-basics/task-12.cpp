#include <cstdio>
#include <mpi.h>

int run(int rank, int size) {

    if (rank == 0)
        std::printf("%d processes.\n", size);
    else
        std::printf("I am %d: %s!\n", rank, rank % 2 == 0 ? "FIRST" : "SECOND");
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    const int result = run(rank, size);
    MPI_Finalize();
    return result;
}
