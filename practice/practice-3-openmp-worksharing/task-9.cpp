#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k) {
    omp_set_dynamic(0);
    #pragma omp parallel num_threads(k)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                std::printf("[%d]: came in section 1\n", omp_get_thread_num());
            }
            #pragma omp section
            {
                std::printf("[%d]: came in section 2\n", omp_get_thread_num());
            }
            #pragma omp section
            {
                std::printf("[%d]: came in section 3\n", omp_get_thread_num());
            }
        }
        std::printf("[%d]: parallel region\n", omp_get_thread_num());
    }
    return 0;
}

int main(int argc, char **argv) {
    int k;
    if (argc == 2) {
        k = std::stoi(argv[1]);
    } else if (argc == 1) {
        std::cout << "Введите количество потоков: ";
        std::cin >> k;
    } else {
        std::cerr << "Использование: task-9 [k]\n";
        return 2;
    }
    return run(k);
}
