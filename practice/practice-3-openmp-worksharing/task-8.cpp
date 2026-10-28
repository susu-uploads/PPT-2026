#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

int run(int n, const std::vector<double> &a, const std::vector<double> &b) {
    std::vector<double> c(n * n);
    omp_set_dynamic(0);
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            double sum = 0;
            for (int k = 0; k < n; ++k)
                sum += a[i * n + k] * b[k * n + j];
            c[i * n + j] = sum;
        }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            std::printf("%.2f%c", c[i * n + j], j + 1 == n ? '\n' : ' ');
    }
    return 0;
}

int main(int argc, char **argv) {
    int n;
    if (argc > 1) {
        n = std::stoi(argv[1]);
        if (argc != 2 + 2 * n * n) {
            std::cerr << "Использование: task-8 [n A... B...]\n";
            return 2;
        }
    } else {
        std::cout << "Введите размер n: ";
        std::cin >> n;
    }
    std::vector<double> a(n * n), b(n * n);
    if (argc > 1) {
        for (int i = 0; i < n * n; ++i)
            a[i] = std::stod(argv[2 + i]);
        for (int i = 0; i < n * n; ++i)
            b[i] = std::stod(argv[2 + n * n + i]);
    } else {
        std::cout << "Введите элементы матрицы A построчно: ";
        for (double &value : a)
            std::cin >> value;
        std::cout << "Введите элементы матрицы B построчно: ";
        for (double &value : b)
            std::cin >> value;
    }
    return run(n, a, b);
}
