#include <chrono>
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <thread>

int run(int k, const std::string &mode) {
    omp_set_dynamic(0);
    int rank = 0;
    if (mode == "all" || mode == "shared") {
        std::puts("Shared:");
        #pragma omp parallel num_threads(k) shared(rank)
        {
            rank = omp_get_thread_num();
            std::printf("I am %d thread.\n", rank);
        }
    }
    if (mode == "all" || mode == "shared-sleep") {
        std::puts("Shared with sleep:");
        #pragma omp parallel num_threads(k) shared(rank)
        {
            rank = omp_get_thread_num();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::printf("I am %d thread.\n", rank);
        }
    }
    if (mode == "all" || mode == "private") {
        std::puts("Private:");
        #pragma omp parallel num_threads(k) private(rank)
        {
            rank = omp_get_thread_num();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::printf("I am %d thread.\n", rank);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    int k;
    std::string mode;
    if (argc == 3) {
        k = std::stoi(argv[1]);
        mode = argv[2];
    } else if (argc == 1) {
        std::cout << "Введите количество потоков и режим (all, shared, shared-sleep, private): ";
        std::cin >> k >> mode;
    } else {
        std::cerr << "Использование: task-4 [k mode]\n";
        return 2;
    }
    return run(k, mode);
}
