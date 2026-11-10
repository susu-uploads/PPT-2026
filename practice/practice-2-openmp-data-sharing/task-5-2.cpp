#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k, int n) {
    omp_set_dynamic(0);
    int sum = 0;
    #pragma omp parallel num_threads(k) reduction(+ : sum)
    {
        const int rank = omp_get_thread_num(), size = omp_get_num_threads();
        const int base = n / size, remainder = n % size;
        const int start = rank * base + (rank < remainder ? rank : remainder) + 1;
        const int end = start + base + (rank < remainder) - 1;
        for (int i = start; i <= end; ++i)
            sum += i;
        std::printf("[%d]: Sum = %d\n", rank, sum);
    }
    std::printf("Sum = %d\n", sum);
    return 0;
}

int main(int argc, char **argv) {
    int k;
    int n;
    if (argc == 3) {
        k = std::stoi(argv[1]);
        n = std::stoi(argv[2]);
    } else if (argc == 1) {
        std::cout << "Введите количество потоков: ";
        std::cin >> k;
        std::cout << "Введите N: ";
        std::cin >> n;
    } else {
        std::cerr << "Использование: task-5-2 [k N]\n";
        return 2;
    }
    return run(k, n);
}
