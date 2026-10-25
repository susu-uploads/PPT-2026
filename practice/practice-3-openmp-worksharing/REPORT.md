# Практическая работа №3

Бабушкин Михаил Вадимович. Технологии параллельного программирования.

## Реализация

task-6 реализует суммирование и зависимое задание 7: расписание задаётся строкой schedule, вывод назначения итераций — параметром trace. Для задания 7 используются k=4 и N=10. task-8 умножает квадратные матрицы одной командой OpenMP. task-9 содержит три секции, затем каждый поток выводит parallel region. Корректные данные предполагают положительное k, N от 0 до 4294967295 и размеры матриц 1..10.

`task-6 k N schedule trace` (static, static,1, static,2, dynamic, dynamic,2, guided, guided,2; trace=0/1); `task-8 n A... B...`; `task-9 k`.

Полный набор параметров передаётся позиционными аргументами. Без аргументов main выводит одно приглашение и читает те же значения через stdin; в MPI-программах ввод читает только ранг 0. Числа преобразуются стандартными функциями, дополнительная валидация содержимого ввода не выполняется.

Исходники находятся непосредственно в каталоге практики. Каждый независимый `.cpp` имеет собственную точку входа; вся практика собирается одной командой CMake.

## ИСХОДНЫЙ КОД

### task-6.cpp

```cpp
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

// Задание 7 использует фиксированные k=4, N=10 и выбранное расписание.
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
    // Настройка будет использована директивой schedule(runtime).
    omp_set_schedule(kind, chunk);
    long long sum = 0;
    // Частные аккумуляторы предотвращают конфликт одновременных прибавлений.
    #pragma omp parallel num_threads(k) reduction(+ : sum)
    {
        const int rank = omp_get_thread_num();
        // Библиотека распределяет итерации между потоками согласно выбранному расписанию.
        #pragma omp for schedule(runtime)
        for (long long i = 1; i <= n; ++i) {
            // Диагностические строки позволяют восстановить фактическое назначение итераций.
            if (trace)
                std::printf("[%d]: calculation of the iteration number %lld\n", rank, i);
            sum += i;
        }
        std::printf("[%d]: Sum = %lld\n", rank, sum);
    }
    // Результат читается после объединения частичных сумм и завершения команды.
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
        std::cout << "Введите k, N, расписание (static, static,1, static,2, dynamic, dynamic,2, guided, guided,2) и вывод итераций (0/1): ";
        std::cin >> k >> n >> schedule >> trace;
    } else {
        std::cerr << "Использование: task-6 [k N schedule trace]\n";
        return 2;
    }
    return run(k, n, schedule, trace);
}
```

### task-8.cpp

```cpp
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

int run(int n, const std::vector<double> &a, const std::vector<double> &b) {
    // Матрицы хранятся непрерывно; элемент (i,j) находится по индексу i*n+j.
    std::vector<double> c(n * n);
    omp_set_dynamic(0);
    // Одна команда распределяет пары (i,j); вложенная команда потоков не создаётся.
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            // Скалярный аккумулятор принадлежит одной итерации и не разделяется потоками.
            double sum = 0;
            for (int k = 0; k < n; ++k)
                sum += a[i * n + k] * b[k * n + j];
            c[i * n + j] = sum;
        }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            // После параллельного цикла все ячейки C готовы; вывод выполняет главный поток.
            std::printf("%.2f%c", c[i * n + j], j + 1 == n ? '\n' : ' ');
    }
    return 0;
}

int main(int argc, char **argv) {
    // По условию размер квадратных матриц ограничен диапазоном 1..10.
    int n;
    if (argc > 1) {
        n = std::stoi(argv[1]);
        if (argc != 2 + 2 * n * n) {
            std::cerr << "Использование: task-8 [n A... B...]\n";
            return 2;
        }
    } else {
        std::cout << "Введите размер n и элементы матриц A и B построчно: ";
        std::cin >> n;
    }
    std::vector<double> a(n * n), b(n * n);
    if (argc > 1) {
        for (int i = 0; i < n * n; ++i)
            a[i] = std::stod(argv[2 + i]);
        for (int i = 0; i < n * n; ++i)
            b[i] = std::stod(argv[2 + n * n + i]);
    } else {
        for (double &value : a)
            std::cin >> value;
        for (double &value : b)
            std::cin >> value;
    }
    return run(n, a, b);
}
```

### task-9.cpp

```cpp
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int run(int k) {
    omp_set_dynamic(0);
    #pragma omp parallel num_threads(k)
    {
        // Каждая секция исполняется ровно один раз; соответствие номерам потоков не фиксируется.
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
        // Здесь все секции уже завершились: в конце sections действует неявный барьер.
        std::printf("[%d]: parallel region\n", omp_get_thread_num());
    }
    return 0;
}

int main(int argc, char **argv) {
    // Параметр k задаёт размер команды, а число секций всегда равно трём.
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
```

## Среда и сборка

Apple M4 Max; 14 физических и логических CPU-ядер, 36 GiB RAM; macOS 15.8, ARM64.

```text
cmake 4.4.4
libomp 23.1.3
llvm 23.1.3_1
open-mpi 5.0.11
```

Команды выполняются из каталога этой практики. Первоначальная установка зависимостей:

```bash
brew install llvm libomp open-mpi cmake
```

```bash
cmake -S . -B .cache/build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="$(brew --prefix llvm)/bin/clang++" \
  -DOpenMP_CXX_INCLUDE_DIR="$(brew --prefix libomp)/include" \
  -DOpenMP_omp_LIBRARY="$(brew --prefix libomp)/lib/libomp.dylib"
cmake --build .cache/build -j 4
```

Число потоков для программ без собственного параметра задаёт `OMP_NUM_THREADS`; динамическое изменение размера команды отключено. MPI-программы запускаются через `mpirun --bind-to none -np P`. Порядок строк от разных исполнителей не фиксируется.

## Проверка

Практика проверяется компиляцией и прямыми запусками бинарей без тестовых файлов и фреймворка. Проверяются результаты вычислений и состав сообщений; порядок строк между потоками и процессами не фиксируется. Проверяются все варианты запуска через CLI и stdin, нечётное распределение работы и пустые диапазоны. Проверяются расписания, суммы, умножение матриц и однократное выполнение каждой из трёх секций.

## Эксперименты

Таблица одного фактического запуска каждой настройки при k=4, N=10. Число в ячейке — номер потока. Повторение: `.cache/build/task-6 4 10 static,1 1` и аналогично для остальных столбцов.

| Итерация | static | static,1 | static,2 | dynamic | dynamic,2 | guided | guided,2 |
|---|---|---|---|---|---|---|---|
| 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| 2 | 0 | 1 | 0 | 2 | 0 | 3 | 0 |
| 3 | 0 | 2 | 1 | 3 | 2 | 2 | 3 |
| 4 | 1 | 3 | 1 | 1 | 2 | 0 | 3 |
| 5 | 1 | 0 | 2 | 0 | 3 | 0 | 2 |
| 6 | 1 | 1 | 2 | 0 | 3 | 0 | 2 |
| 7 | 2 | 2 | 3 | 0 | 0 | 0 | 1 |
| 8 | 2 | 3 | 3 | 0 | 0 | 0 | 1 |
| 9 | 3 | 0 | 0 | 0 | 0 | 0 | 0 |
| 10 | 3 | 1 | 0 | 1 | 0 | 1 | 0 |

Для dynamic/guided таблица отражает конкретное выполнение, а не постоянное распределение.

```bash
OMP_NUM_THREADS=4 .cache/build/task-6 3 2 static 0
```

```text
[0]: Sum = 1
[2]: Sum = 0
[1]: Sum = 2
Sum = 3
```

```bash
OMP_NUM_THREADS=4 .cache/build/task-8 2 1 1 4 4 5 5 3 3
```

```text
8.00 8.00
32.00 32.00
```

```bash
OMP_NUM_THREADS=4 .cache/build/task-9 2
```

```text
[0]: came in section 1
[0]: came in section 2
[1]: came in section 3
[0]: parallel region
[1]: parallel region
```

```bash
OMP_NUM_THREADS=4 .cache/build/task-9 3
```

```text
[0]: came in section 1
[1]: came in section 2
[2]: came in section 3
[0]: parallel region
[2]: parallel region
[1]: parallel region
```

```bash
OMP_NUM_THREADS=4 .cache/build/task-9 4
```

```text
[0]: came in section 1
[2]: came in section 3
[1]: came in section 2
[0]: parallel region
[3]: parallel region
[2]: parallel region
[1]: parallel region
```

## Контрольные вопросы

### 1. Почему в OpenMP реализован только цикл for? Любой ли корректный последовательный цикл for можно распараллелить?

Директива omp for требует канонический цикл с вычислимыми границами, шагом и счётчиком. Произвольный корректный последовательный for не обязательно подходит: зависимости между итерациями могут нарушить результат. while нельзя непосредственно пометить omp for. Современный стандарт также содержит другие средства параллелизма, включая tasks и loop.

### 2. На каком этапе выполняется распределение итераций по нитям, опишите алгоритм этого распределения.

Итерации распределяет библиотека выполнения при входе команды в цикл. static заранее задаёт распределение блоков, dynamic выдаёт следующий блок освободившемуся потоку, guided уменьшает размер выдаваемых блоков по мере выполнения. Фактическое распределение для этого запуска приведено в таблице.

### 3. Опишите назначение параметра schedule и смысл его двух параметров.

schedule(kind, chunk) определяет алгоритм распределения и размер блока. static без размера делит диапазон на близкие по размеру части; static с размером циклически раздаёт блоки. Для dynamic размер по умолчанию равен 1. Для guided размер задаёт нижнюю границу блока, кроме последнего остатка. В программе schedule(runtime) использует выбранную через omp_set_schedule настройку.

### 4. Для чего введены динамические распределения итераций?

Динамическое распределение полезно, когда стоимость итераций неодинакова или нагрузка меняется. Освободившийся поток получает следующую порцию. Цена — дополнительные обращения к планировщику; для одинаковых коротких итераций static обычно требует меньше служебной работы.

### 5. В каких случаях и почему распределение типа guided будет работать эффективнее, чем dynamic, и наоборот?

guided сначала выдаёт крупные блоки и уменьшает их, сокращая число выдач по сравнению с dynamic с мелким блоком. dynamic может лучше балансировать резко неравномерную работу. Для десяти демонстрационных итераций вывод показывает назначение потоков, но не доказывает превосходство одного расписания по скорости.

### 6. Для чего предназначена директива sections?

sections распределяет независимые блоки section между потоками команды. В task-9 присутствуют три секции; каждая выполняется ровно один раз, независимо от размера команды.

### 7. В какой момент секции кода распределяются по нитям?

Секции назначаются во время выполнения при достижении конструкции sections. Конкретное соответствие между секцией и номером потока определяется реализацией и текущим выполнением.

### 8. Почему в OpenMP не реализовано статическое распределение секций? Как это можно сделать?

sections описывает набор независимых блоков без гарантированного соответствия номерам потоков. Если такое соответствие нужно, можно явно ветвиться по omp_get_thread_num() или представить работу циклом с подходящим static-расписанием. В этой работе назначение секций намеренно оставлено библиотеке.

### 9. В каком порядке нити получают секции кода?

Гарантированного порядка выдачи секций потокам нет. Даже порядок появления строк не является точным порядком начала выполнения секций.

### 10. Получат ли нити одинаковое количество секций?

Нет. При двух потоках кто-то выполнит несколько секций; при четырёх потоках как минимум один не получит секцию. Даже при трёх потоках стандарт не требует по одной секции каждому. После неявного барьера каждый поток печатает сообщение parallel region.

## Вывод

Все семь расписаний покрывают итерации 1..10 ровно по одному разу и дают сумму 55. Динамические назначения могут меняться между запусками. Произведения матриц совпали с эталонами; число выполнений секций всегда равно трём, а сообщений parallel region — числу потоков.

## Источники

- [Лабораторная работа №3.1.pdf](<Лабораторная работа №3.1.pdf>).
- [Лабораторная работа №3.2.pdf](<Лабораторная работа №3.2.pdf>).
- [Типовые контрольные вопросы №3.docx](<Типовые контрольные вопросы №3.docx>): исходные контрольные вопросы.
- [Спецификации OpenMP](https://www.openmp.org/specifications/) и [документация Open MPI](https://docs.open-mpi.org/en/v5.0.x/). Ответы учитывают используемую среду macOS.
