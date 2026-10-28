#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int run(int n, std::vector<double> &a, std::vector<double> &b, bool point, bool benchmark, int rank, int size) {
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int rows = (n + size - 1) / size, count = rows * n;
    a.resize(count * size);
    b.resize(n * n);
    std::vector<double> c(count * size), local_a(count), local_c(count);
    const int repeats = benchmark ? 1000 : 1;
    if (benchmark)
        MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    for (int repeat = 0; repeat < repeats; ++repeat) {
        if (point) {
            if (rank == 0) {
                std::copy_n(a.begin(), count, local_a.begin());
                for (int p = 1; p < size; ++p) {
                    MPI_Send(a.data() + p * count, count, MPI_DOUBLE, p, 1, MPI_COMM_WORLD);
                    MPI_Send(b.data(), n * n, MPI_DOUBLE, p, 2, MPI_COMM_WORLD);
                }
            } else {
                MPI_Recv(local_a.data(), count, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(b.data(), n * n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        } else {
            MPI_Scatter(a.data(), count, MPI_DOUBLE, local_a.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Bcast(b.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < n; ++j) {
                double sum = 0;
                for (int k = 0; k < n; ++k)
                    sum += local_a[i * n + k] * b[k * n + j];
                local_c[i * n + j] = sum;
            }
        if (point) {
            if (rank == 0) {
                std::copy(local_c.begin(), local_c.end(), c.begin());
                for (int p = 1; p < size; ++p)
                    MPI_Recv(c.data() + p * count, count, MPI_DOUBLE, p, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else
                MPI_Send(local_c.data(), count, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        } else
            MPI_Gather(local_c.data(), count, MPI_DOUBLE, c.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    if (benchmark) {
        const double elapsed = MPI_Wtime() - start;
        double maximum;
        MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0)
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    if (rank == 0)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                std::printf("%.2f%c", c[i * n + j], j + 1 == n ? '\n' : ' ');
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = 0, point = 0, benchmark = 0, status = 0;
    std::vector<double> a, b;
    if (rank == 0) {
        if (argc > 1) {
            n = std::stoi(argv[1]);
            if (argc != 4 + 2 * n * n) {
                std::cerr << "Использование: task-19 [n point benchmark A... B...]\n";
                status = 2;
            } else {
                point = std::stoi(argv[2]);
                benchmark = std::stoi(argv[3]);
            }
        } else {
            std::cout << "Введите размер n:\n";
            std::cin >> n;
            std::cout << "Использовать точечные обмены (0/1):\n";
            std::cin >> point;
            std::cout << "Измерять время (0/1):\n";
            std::cin >> benchmark;
        }
        if (status == 0) {
            a.resize(n * n);
            b.resize(n * n);
            if (argc > 1) {
                for (int i = 0; i < n * n; ++i)
                    a[i] = std::stod(argv[4 + i]);
                for (int i = 0; i < n * n; ++i)
                    b[i] = std::stod(argv[4 + n * n + i]);
            } else {
                std::cout << "Введите элементы матрицы A построчно:\n";
                for (double &value : a)
                    std::cin >> value;
                std::cout << "Введите элементы матрицы B построчно:\n";
                for (double &value : b)
                    std::cin >> value;
            }
        }
    }
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (status != 0) {
        MPI_Finalize();
        return status;
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, a, b, point, benchmark, rank, size);
    MPI_Finalize();
    return result;
}
