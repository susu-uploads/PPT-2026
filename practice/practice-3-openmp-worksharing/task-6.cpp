#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k, long long n, const std::string &schedule, bool trace) {
    int chunk = 0;
    omp_sched_t kind = omp_sched_static;
    if (schedule == "static")
        kind = omp_sched_static;
    else if (schedule == "static,1") {
        kind = omp_sched_static;
        chunk = 1;
    } else if (schedule == "static,2") {
        kind = omp_sched_static;
        chunk = 2;
    } else if (schedule == "dynamic") {
        kind = omp_sched_dynamic;
        chunk = 1;
    } else if (schedule == "dynamic,2") {
        kind = omp_sched_dynamic;
        chunk = 2;
    } else if (schedule == "guided") {
        kind = omp_sched_guided;
        chunk = 1;
    } else if (schedule == "guided,2") {
        kind = omp_sched_guided;
        chunk = 2;
    }
    omp_set_dynamic(0);
    omp_set_schedule(kind, chunk);
    long long sum = 0;
    #pragma omp parallel num_threads(k) reduction(+ : sum)
    {
        const int rank = omp_get_thread_num();
        #pragma omp for schedule(runtime)
        for (long long i = 1; i <= n; ++i) {
            if (trace)
                std::printf("[%d]: calculation of the iteration number %lld\n", rank, i);
            sum += i;
        }
        std::printf("[%d]: Sum = %lld\n", rank, sum);
    }
    std::printf("Sum = %lld\n", sum);
    return 0;
}

int main(int argc, char **argv) {
    int k;
    long long n;
    std::string schedule;
    bool trace;
    if (argc == 5) {
        k = std::stoi(argv[1]);
        n = std::stoll(argv[2]);
        schedule = argv[3];
        trace = std::stoi(argv[4]);
    } else if (argc == 1) {
        std::cout << "Введите количество потоков: ";
        std::cin >> k;
        std::cout << "Введите N: ";
        std::cin >> n;
        std::cout << "Введите расписание (static, static,1, static,2, dynamic, dynamic,2, guided, guided,2): ";
        std::cin >> schedule;
        std::cout << "Выводить итерации (0/1): ";
        std::cin >> trace;
    } else {
        std::cerr << "Использование: task-6 [k N schedule trace]\n";
        return 2;
    }
    return run(k, n, schedule, trace);
}
