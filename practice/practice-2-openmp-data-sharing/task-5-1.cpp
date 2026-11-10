#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int n) {
    omp_set_dynamic(0);
    int sum = 0;
    #pragma omp parallel num_threads(2) reduction(+ : sum)
    {
        const int rank = omp_get_thread_num();
        int start, end;
        if (rank == 0) {
            start = 1;
            end = n / 2;
        } else {
            start = n / 2 + 1;
            end = n;
        }
        for (int i = start; i <= end; ++i)
            sum += i;
        std::printf("[%d]: Sum = %d\n", rank, sum);
    }
    std::printf("Sum = %d\n", sum);
    return 0;
}

int main(int argc, char **argv) {
    int n;
    if (argc == 2) {
        n = std::stoi(argv[1]);
    } else if (argc == 1) {
        std::cout << "Введите N: ";
        std::cin >> n;
    } else {
        std::cerr << "Использование: task-5-1 [N]\n";
        return 2;
    }
    return run(n);
}
