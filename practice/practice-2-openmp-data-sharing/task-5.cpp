#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k, long long n, bool extra) {
    omp_set_dynamic(0);
    long long sum = 0;
    #pragma omp parallel num_threads(k) reduction(+ : sum)
    {
        const int rank = omp_get_thread_num(), size = omp_get_num_threads();
        long long start, end;
        if (!extra && rank == 0) {
            start = 1;
            end = n / 2;
        } else if (!extra) {
            start = n / 2 + 1;
            end = n;
        } else {
            const long long base = n / size, remainder = n % size;
            start = rank * base + (rank < remainder ? rank : remainder) + 1;
            end = start + base + (rank < remainder) - 1;
        }
        for (long long i = start; i <= end; ++i)
            sum += i;
        std::printf("[%d]: Sum = %lld\n", rank, sum);
    }
    std::printf("Sum = %lld\n", sum);
    return 0;
}

int main(int argc, char **argv) {
    int k;
    long long n;
    bool extra;
    if (argc == 4) {
        k = std::stoi(argv[1]);
        n = std::stoll(argv[2]);
        extra = std::stoi(argv[3]);
    } else if (argc == 1) {
        std::cout << "Введите количество потоков: ";
        std::cin >> k;
        std::cout << "Введите N: ";
        std::cin >> n;
        std::cout << "Введите вариант (0 — базовый с k=2, 1 — дополнительный): ";
        std::cin >> extra;
    } else {
        std::cerr << "Использование: task-5 [k N extra]\n";
        return 2;
    }
    return run(k, n, extra);
}
