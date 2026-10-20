#include <cstdio>
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
  int n;
  if (!(std::cin >> n) || n < 1 || n > 10) {
    std::cerr << "Размер матриц: 1..10.\n";
    return 2;
  }
  std::vector<double> a(n * n), b(n * n), c(n * n);
  for (double &x : a)
    if (!(std::cin >> x)) {
      std::cerr << "Неполная матрица A.\n";
      return 2;
    }
  for (double &x : b)
    if (!(std::cin >> x)) {
      std::cerr << "Неполная матрица B.\n";
      return 2;
    }
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
}
