---
ru: "Задание №9"
en: "Assignment 9"
code: "practice-10-assignment-9"
origin: "https://edu.susu.ru/mod/assign/view.php?id=8720543"
---

Напишите ядро для вычисления числа 𝜋 из предыдущего раздела и сравните время выполнения. Используйте за основу следующую фунцию.
double CalcPi(const int n)
{
double pi = 0;
const double coef = 1.0 / n;

for (int i = 0; i < n; ++i)
{
const double xi = (i + 0.5) * coef;
pi += 4.0 / (1.0 + xi * xi);
}
return pi * coef;
}

Выполняйте все итерации на устройстве.
