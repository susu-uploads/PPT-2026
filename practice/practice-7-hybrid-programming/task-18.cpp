#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int run(long long n, bool point, bool benchmark, int rank, int size) {
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_LONG_LONG_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
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
                MPI_Recv(&value, 1, MPI_DOUBLE, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    if (rank == 0)
        std::printf("pi = %.8f\n", total * h);
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 1 && argc != 4) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-18.\n";
        MPI_Finalize();
        return 2;
    }
    long long n = 0;
    int point = 0, benchmark = 0;
    if (rank == 0) {
        if (argc == 4) {
            n = std::stoll(argv[1]);
            point = std::stoi(argv[2]);
            benchmark = std::stoi(argv[3]);
        } else {
            std::cout << "Введите N:\n";
            std::cin >> n;
            std::cout << "Использовать точечные обмены (0/1):\n";
            std::cin >> point;
            std::cout << "Измерять время (0/1):\n";
            std::cin >> benchmark;
        }
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, point, benchmark, rank, size);
    MPI_Finalize();
    return result;
}
