#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int run(int n, const std::string &text, bool point, bool benchmark, int rank, int size) {
    char buffer[101] = {};
    if (rank == 0)
        std::copy(text.begin(), text.end(), buffer);
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int repeats = benchmark ? 1000 : 1;
    int counts[26] = {};
    if (benchmark)
        MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    for (int repeat = 0; repeat < repeats; ++repeat) {
        if (point) {
            if (rank == 0)
                for (int p = 1; p < size; ++p)
                    MPI_Send(buffer, n, MPI_CHAR, p, 1, MPI_COMM_WORLD);
            else
                MPI_Recv(buffer, n, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else
            MPI_Bcast(buffer, n, MPI_CHAR, 0, MPI_COMM_WORLD);
        std::fill(counts, counts + 26, 0);
        for (int letter = rank; letter < 26; letter += size)
            for (int i = 0; i < n; ++i)
                if (buffer[i] == 'a' + letter)
                    ++counts[letter];
    }

    if (benchmark) {
        const double elapsed = MPI_Wtime() - start;
        double maximum;
        MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0)
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    for (int letter = rank; letter < 26; letter += size)
        if (counts[letter])
            std::printf("%c = %d\n", 'a' + letter, counts[letter]);
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 1 && argc != 5) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-17.\n";
        MPI_Finalize();
        return 2;
    }
    int n = 0, point = 0, benchmark = 0;
    std::string text;
    if (rank == 0) {
        if (argc == 5) {
            n = std::stoi(argv[1]);
            text = argv[2];
            point = std::stoi(argv[3]);
            benchmark = std::stoi(argv[4]);
        } else {
            std::cout << "Введите длину строки:\n";
            std::cin >> n;
            std::cout << "Введите строку:\n";
            std::cin >> text;
            std::cout << "Использовать точечные обмены (0/1):\n";
            std::cin >> point;
            std::cout << "Измерять время (0/1):\n";
            std::cin >> benchmark;
        }
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, text, point, benchmark, rank, size);
    MPI_Finalize();
    return result;
}
