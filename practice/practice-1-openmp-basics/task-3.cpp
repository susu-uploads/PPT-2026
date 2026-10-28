#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k, bool even) {
    omp_set_dynamic(0);
    #pragma omp parallel num_threads(k)
    {
        const int rank = omp_get_thread_num();
        if (!even || rank % 2 == 0) {
            std::printf("I am %d thread from %d threads!\n", rank, omp_get_num_threads());
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    int k;
    bool even;

    if (argc == 3) {
        k = std::stoi(argv[1]);
        even = std::stoi(argv[2]);
    } else if (argc == 1) {
        std::cout << "Введите количество потоков: ";
        std::cin >> k;
        std::cout << "Введите режим (0 — все, 1 — только чётные): ";
        std::cin >> even;
    } else {
        std::cerr << "Использование: task-3 [k even]\n";
        return 2;
    }

    return run(k, even);
}
