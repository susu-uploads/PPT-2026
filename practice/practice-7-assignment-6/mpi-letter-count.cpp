#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  bool point = false, benchmark = false;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--point-to-point")
      point = true;
    else if (arg == "--benchmark")
      benchmark = true;
    else {
      std::cerr << "Флаги: --point-to-point, --benchmark.\n";
      return 2;
    }
  }
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n = 0;
  char buffer[101] = {};
  if (rank == 0) {
    std::string input;
    if (!(std::cin >> n >> input) || n < 1 || n > 100 ||
        input.size() != static_cast<size_t>(n) ||
        !std::all_of(input.begin(), input.end(),
                     [](char c) { return c >= 'a' && c <= 'z'; })) {
      n = 0;
      std::cerr << "Нужны длина 1..100 и строка этой длины из a..z.\n";
    } else
      std::copy(input.begin(), input.end(), buffer);
  }
  if (point) {
    if (rank == 0)
      for (int p = 1; p < size; ++p)
        MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
    else
      MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (!n) {
    MPI_Finalize();
    return 2;
  }
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
      std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats,
                  repeats);
  }

  for (int letter = rank; letter < 26; letter += size)
    if (counts[letter])
      std::printf("%c = %d\n", 'a' + letter, counts[letter]);

  MPI_Finalize();
}
