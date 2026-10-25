#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>

int run(int k, int rank, int size) {
    omp_set_dynamic(0);
    #pragma omp parallel num_threads(k)
    {
        std::printf("I am %d thread from %d process. Number of hybrid threads = %d.\n", omp_get_thread_num(), rank, omp_get_num_threads() * size);
    }
    return 0;
}

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
    if (argc != 1 && argc != 2) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-31.\n";
        MPI_Finalize();
        return 2;
    }
    int k = 0;
    if (rank == 0) {
        if (argc == 2) {
            k = std::stoi(argv[1]);
        } else {
            std::cout << "Введите количество потоков в каждом процессе: ";
            std::cin >> k;
        }
    }
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(k, rank, size);
    MPI_Finalize();
    return result;
}
