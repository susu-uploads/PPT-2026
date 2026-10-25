#include <cstdio>
#include <mpi.h>

int run(int rank, int size) {

    std::printf("I am %d process from %d processes!\n", rank, size);
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
