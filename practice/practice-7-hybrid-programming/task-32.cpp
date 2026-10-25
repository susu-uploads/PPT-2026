#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>

int run(long long n, const std::string &mode, int k, int rank, int size) {
    omp_set_dynamic(0);
    omp_set_num_threads(k);
    MPI_Bcast(&n, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
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
        std::printf("MPI processes = %d, OpenMP threads per process = %d\n", size, mode == "serial" ? 1 : omp_get_max_threads());
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
    if (argc != 1 && argc != 4) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-32.\n";
        MPI_Finalize();
        return 2;
    }
    long long n = 0;
    std::string mode;
    int k = 0;
    if (rank == 0) {
        if (argc == 4) {
            n = std::stoll(argv[1]);
            mode = argv[2];
            k = std::stoi(argv[3]);
        } else {
            std::cout << "Введите N, режим (serial, openmp, hybrid) и количество потоков: ";
            std::cin >> n >> mode >> k;
        }
    }
    int mode_length = static_cast<int>(mode.size());
    MPI_Bcast(&mode_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    mode.resize(mode_length);
    MPI_Bcast(mode.data(), mode_length, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, mode, k, rank, size);
    MPI_Finalize();
    return result;
}
