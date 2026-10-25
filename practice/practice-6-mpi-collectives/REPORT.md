# Практическая работа №6

Бабушкин Михаил Вадимович. Технологии параллельного программирования.

## Реализация

task-17 считает буквы a..z, task-18 интегрирует 4/(1+x²) на [0,1], task-19 умножает квадратные матрицы. point=0 выбирает коллективные операции, point=1 — точечные обмены; benchmark=1 включает измерения с прежними границами и числом повторений. Параметры получает только ранг 0; управляющие режимы согласуются до run, а учебные передачи размера и данных остаются внутри run. Матрицы дополняются нулевыми строками. Предполагается корректный ввод: n=1..100 и строка этой длины из a..z, N>0, размер матриц 1..10. В примере задания 17 n=3 не соответствует строке rvtabc длины 6; для запусков используются согласованные значения.

`task-17 n text point benchmark`; `task-18 N point benchmark`; `task-19 n point benchmark A... B...`. point и benchmark — значения 0/1. Матрицы передаются построчно.

Полный набор параметров передаётся позиционными аргументами. Без аргументов main выводит одно приглашение и читает те же значения через stdin; в MPI-программах ввод читает только ранг 0. Числа преобразуются стандартными функциями, дополнительная валидация содержимого ввода не выполняется.

Исходники находятся непосредственно в каталоге практики. Каждый независимый `.cpp` имеет собственную точку входа; вся практика собирается одной командой CMake.

## ИСХОДНЫЙ КОД

### task-17.cpp

```cpp
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

// Параметры point и benchmark выбирают точечные обмены и режим измерения времени.
int run(int n, const std::string &text, bool point, bool benchmark, int rank, int size) {
    char buffer[101] = {};
    if (rank == 0)
        std::copy(text.begin(), text.end(), buffer);
    // Первая передача сообщает длину строки всем процессам.
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Для измерения короткая операция повторяется 1000 раз; время затем делится на это число.
    const int repeats = benchmark ? 1000 : 1;
    int counts[26] = {};
    if (benchmark)
        MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    // В каждом повторении строка рассылается заново выбранным способом.
    for (int repeat = 0; repeat < repeats; ++repeat) {
        if (point) {
            if (rank == 0)
                for (int p = 1; p < size; ++p)
                    MPI_Send(buffer, n, MPI_CHAR, p, 1, MPI_COMM_WORLD);
            else
                MPI_Recv(buffer, n, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else
            MPI_Bcast(buffer, n, MPI_CHAR, 0, MPI_COMM_WORLD);
        // Счётчики очищаются, чтобы повторы не накапливали частоты.
        std::fill(counts, counts + 26, 0);
        // Буквы алфавита циклически распределены по рангам; каждую букву считает один процесс.
        for (int letter = rank; letter < 26; letter += size)
            for (int i = 0; i < n; ++i)
                if (buffer[i] == 'a' + letter)
                    ++counts[letter];
    }

    if (benchmark) {
        const double elapsed = MPI_Wtime() - start;
        double maximum;
        // Вне измеряемого участка определяется наибольшее время среди процессов.
        MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0)
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    for (int letter = rank; letter < 26; letter += size)
        if (counts[letter])
            std::printf("%c = %d\n", 'a' + letter, counts[letter]);
    return 0;
}

// Длина и буфер строки передаются всем процессам; ввод читает только ранг 0.
int main(int argc, char **argv) {
    // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 1 && argc != 5) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-17.\n";
        MPI_Finalize();
        return 2;
    }
    int n = 0, point = 0, benchmark = 0;
    std::string text;
    if (rank == 0) {
        if (argc == 5) {
            n = std::stoi(argv[1]);
            text = argv[2];
            point = std::stoi(argv[3]);
            benchmark = std::stoi(argv[4]);
        } else {
            std::cout << "Введите длину, строку, точечные обмены (0/1) и измерение времени (0/1): ";
            std::cin >> n >> text >> point >> benchmark;
        }
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, text, point, benchmark, rank, size);
    // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
    MPI_Finalize();
    return result;
}
```

### task-18.cpp

```cpp
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

// Основной вариант использует коллективные операции, дополнительный — точечные.
int run(long long n, bool point, bool benchmark, int rank, int size) {
    // Размер задачи сообщается всем процессам выбранным способом.
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_LONG_LONG_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    // Смежные диапазоны отличаются не более чем на одну итерацию; остаток распределён первым рангам.
    const long long base = n / size, remainder = n % size;
    const long long begin = rank * base + std::min<long long>(rank, remainder);
    const long long end = begin + base + (rank < remainder);
    const int repeats = 1;
    if (benchmark)
        MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    double local = 0, total = 0;
    // Метод средних прямоугольников интегрирует 4/(1+x*x) на отрезке [0,1].
    const double h = 1.0 / n;
    // Каждый процесс вычисляет только свою частичную сумму.
    for (long long i = begin; i < end; ++i) {
        const double x = (i + 0.5) * h;
        local += 4.0 / (1.0 + x * x);
    }
    if (point) {
        if (rank == 0) {
            total = local;
            for (int p = 1; p < size; ++p) {
                double value;
                MPI_Recv(&value, 1, MPI_DOUBLE, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                total += value;
            }
        } else
            MPI_Send(&local, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    } else
        // В коллективном варианте частичные суммы складываются на корневом процессе.
        MPI_Reduce(&local, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (benchmark) {
        const double elapsed = MPI_Wtime() - start;
        double maximum;
        // Максимум времени измеряется отдельной служебной редукцией после вычисления.
        MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0)
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    if (rank == 0)
        // Только корень располагает общей суммой и выводит итоговое приближение pi.
        std::printf("pi = %.8f\n", total * h);
    return 0;
}

int main(int argc, char **argv) {
    // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 1 && argc != 4) {
        if (rank == 0)
            std::cerr << "Нужен полный набор параметров task-18.\n";
        MPI_Finalize();
        return 2;
    }
    // N читает ранг 0 до начала вычислений.
    long long n = 0;
    int point = 0, benchmark = 0;
    if (rank == 0) {
        if (argc == 4) {
            n = std::stoll(argv[1]);
            point = std::stoi(argv[2]);
            benchmark = std::stoi(argv[3]);
        } else {
            std::cout << "Введите N, точечные обмены (0/1) и измерение времени (0/1): ";
            std::cin >> n >> point >> benchmark;
        }
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, point, benchmark, rank, size);
    // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
    MPI_Finalize();
    return result;
}
```

### task-19.cpp

```cpp
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>

// Режим выбирает коллективное или точечное распределение и сбор блоков.
int run(int n, std::vector<double> &a, std::vector<double> &b, bool point, bool benchmark, int rank, int size) {
    // Все процессы получают размер матриц выбранным способом.
    if (point) {
        if (rank == 0)
            for (int p = 1; p < size; ++p)
                MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
        else
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Число строк округляется вверх, чтобы Scatter/Gather использовали равные блоки.
    const int rows = (n + size - 1) / size, count = rows * n;
    // Дополнительные строки заполняются нулями; после сбора они не выводятся.
    a.resize(count * size);
    b.resize(n * n);
    std::vector<double> c(count * size), local_a(count), local_c(count);
    const int repeats = benchmark ? 1000 : 1;
    if (benchmark)
        MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();
    for (int repeat = 0; repeat < repeats; ++repeat) {
        if (point) {
            if (rank == 0) {
                std::copy_n(a.begin(), count, local_a.begin());
                for (int p = 1; p < size; ++p) {
                    MPI_Send(a.data() + p * count, count, MPI_DOUBLE, p, 1, MPI_COMM_WORLD);
                    MPI_Send(b.data(), n * n, MPI_DOUBLE, p, 2, MPI_COMM_WORLD);
                }
            } else {
                MPI_Recv(local_a.data(), count, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(b.data(), n * n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        } else {
            // Каждый процесс получает свой непрерывный блок строк матрицы A.
            MPI_Scatter(a.data(), count, MPI_DOUBLE, local_a.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            // Матрица B нужна каждому процессу целиком для вычисления всех столбцов своего блока.
            MPI_Bcast(b.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
        // Процесс вычисляет строки C только из своего блока A и общей копии B.
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < n; ++j) {
                double sum = 0;
                for (int k = 0; k < n; ++k)
                    sum += local_a[i * n + k] * b[k * n + j];
                local_c[i * n + j] = sum;
            }
        if (point) {
            if (rank == 0) {
                std::copy(local_c.begin(), local_c.end(), c.begin());
                for (int p = 1; p < size; ++p)
                    MPI_Recv(c.data() + p * count, count, MPI_DOUBLE, p, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else
                MPI_Send(local_c.data(), count, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        } else
            // Блоки C собираются на корне в порядке рангов, включая нулевое дополнение.
            MPI_Gather(local_c.data(), count, MPI_DOUBLE, c.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    if (benchmark) {
        const double elapsed = MPI_Wtime() - start;
        double maximum;
        // Служебное измерение максимума времени не входит в измеряемый участок.
        MPI_Reduce(&elapsed, &maximum, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (rank == 0)
            std::printf("Time = %.12f\nRepetitions = %d\n", maximum / repeats, repeats);
    }

    // Выводятся только исходные n строк, а строки дополнения отбрасываются.
    if (rank == 0)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                std::printf("%.2f%c", c[i * n + j], j + 1 == n ? '\n' : ' ');
    return 0;
}

// Ранг 0 читает размер и обе матрицы.
int main(int argc, char **argv) {
    // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = 0, point = 0, benchmark = 0, status = 0;
    std::vector<double> a, b;
    if (rank == 0) {
        if (argc > 1) {
            n = std::stoi(argv[1]);
            if (argc != 4 + 2 * n * n) {
                std::cerr << "Использование: task-19 [n point benchmark A... B...]\n";
                status = 2;
            } else {
                point = std::stoi(argv[2]);
                benchmark = std::stoi(argv[3]);
            }
        } else {
            std::cout << "Введите размер n, точечные обмены (0/1), измерение времени (0/1) и матрицы A и B построчно: ";
            std::cin >> n >> point >> benchmark;
        }
        if (status == 0) {
            a.resize(n * n);
            b.resize(n * n);
            if (argc > 1) {
                for (int i = 0; i < n * n; ++i)
                    a[i] = std::stod(argv[4 + i]);
                for (int i = 0; i < n * n; ++i)
                    b[i] = std::stod(argv[4 + n * n + i]);
            } else {
                for (double &value : a)
                    std::cin >> value;
                for (double &value : b)
                    std::cin >> value;
            }
        }
    }
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (status != 0) {
        MPI_Finalize();
        return status;
    }
    MPI_Bcast(&point, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&benchmark, 1, MPI_INT, 0, MPI_COMM_WORLD);
    const int result = run(n, a, b, point, benchmark, rank, size);
    // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
    MPI_Finalize();
    return result;
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
  -DCMAKE_CXX_COMPILER="$(brew --prefix llvm)/bin/clang++"
cmake --build .cache/build -j 4
```

Число потоков для программ без собственного параметра задаёт `OMP_NUM_THREADS`; динамическое изменение размера команды отключено. MPI-программы запускаются через `mpirun --bind-to none -np P`. Порядок строк от разных исполнителей не фиксируется.

## Проверка

Практика проверяется компиляцией и прямыми запусками бинарей без тестовых файлов и фреймворка. Проверяются результаты вычислений и состав сообщений; порядок строк между потоками и процессами не фиксируется. Проверяются все варианты запуска через CLI и stdin, нечётное распределение работы и пустые диапазоны. MPI-программы запускаются с 1, 2 и 4 процессами. Проверяются частоты букв, π, матрицы и оба способа обмена. Короткие запуски режима benchmark подтверждают сохранение формата времени и повторений.

## Эксперименты


```bash
mpirun --bind-to none -np 4 .cache/build/task-17 11 abracadabra 0 0
```

```text
a = 5
c = 1
b = 2
r = 2
d = 1
```

```bash
mpirun --bind-to none -np 4 .cache/build/task-18 1000000 0 0
```

```text
pi = 3.14159265
```

```bash
mpirun --bind-to none -np 4 .cache/build/task-19 2 0 0 1 3 4 8 5 4 3 0
```

```text
14.00 4.00
44.00 16.00
```

### Сравнение коллективных и точечных обменов

Один прогрев отдельным запуском, затем пять запусков; показана медиана. Запуск процессов, ввод, вывод и первоначальная рассылка размеров исключены. Внутри измеряемого участка — передача данных, вычисление и сбор результата; берётся максимум времени процессов. Для строки и матрицы каждый запуск выполняет 1000 повторений, в таблице время одного повторения. Для π — один расчёт. Служебная редукция времени находится за пределами измеряемого участка.

Повторение: `python3 benchmark.py`. Исходные пять значений каждой серии сохраняются в `.cache/measurements.json`.

| Данные | P | Коллективные, с | Точечные, с |
|---|---:|---:|---:|
| строка, n=100 | 1 | 0.000000877 | 0.000000864 |
| строка, n=100 | 2 | 0.000003425 | 0.000003294 |
| строка, n=100 | 4 | 0.000004689 | 0.000004699 |
| pi, N=1000000 | 1 | 0.000601000 | 0.000594000 |
| pi, N=1000000 | 2 | 0.000320000 | 0.000315000 |
| pi, N=1000000 | 4 | 0.000169000 | 0.000159000 |
| pi, N=100000000 | 1 | 0.054963000 | 0.056759000 |
| pi, N=100000000 | 2 | 0.027944000 | 0.028144000 |
| pi, N=100000000 | 4 | 0.015396000 | 0.015480000 |
| матрица, n=10 | 1 | 0.000000367 | 0.000000346 |
| матрица, n=10 | 2 | 0.000006603 | 0.000005704 |
| матрица, n=10 | 4 | 0.000006036 | 0.000005173 |

Дата измерений: 2026-10-13T10:25:52.118492+05:00. Строка содержит первые 100 символов повторённого алфавита; A — числа 1..100, B — единичная матрица 10×10. При P=1 обмен с другими процессами отсутствует. Увеличение P для таких малых строки и матрицы дороже последовательного исполнения. Для π при N=10⁸ обе схемы на четырёх процессах ускорились примерно в 3,6 раза относительно своего P=1; разница схем мала и зависит от накладных расходов. Это не измерение сетевой масштабируемости.

## Контрольные вопросы

### 1. В чем отличие широковещательной коммуникации от коммуникации "точка-точка"?

Точечный обмен связывает конкретного отправителя и получателя. Широковещательная операция задаётся всей группой коммуникатора: все участники вызывают Bcast с согласованными параметрами, а данные берутся у root. Реализация может использовать более сложную схему пересылок, чем последовательные посылки от корня.

### 2. Опишите назначение и параметры функции MPI_Bcast.

MPI_Bcast(buffer, count, datatype, root, comm) распространяет count элементов от root всем участникам comm. У корня buffer содержит исходные данные, у остальных — принимает их. В task-17 отдельно сообщается длина строки, затем передаётся ровно n символов.

### 3. Опишите назначение и параметры функции MPI_Reduce. Какие агрегирующие операции могут использоваться в функции MPI_Reduce? В чем отличие работы этой функции в корневом процессе и в остальных процессах коммуникатора?

MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm) объединяет элементы буферов всех процессов. Примеры операций: MPI_SUM, MPI_PROD, MPI_MIN, MPI_MAX и логические операции для подходящих типов. Итоговый recvbuf значим у root; остальные вносят свой sendbuf. Для π суммируются частичные интегральные суммы.

### 4. Опишите назначение и параметры функций MPI_Scatter и MPI_Gather.

MPI_Scatter раздаёт равные блоки корневого sendbuf процессам, MPI_Gather собирает равные блоки обратно в порядке рангов. В task-19 строки A распределяются через Scatter, B целиком рассылается через Bcast, строки C собираются через Gather. Передаваемый блок содержит rows*n элементов MPI_DOUBLE.

### 5. Можно ли с помощью этих функций рассылать/собирать динамически выделенные двумерные массивы (матрицы)? Можно ли использовать эти функции, если количество данных не кратно количеству нитей?

Динамическая память подходит, если передаваемые элементы расположены непрерывно; vector<double> это обеспечивает. Массив указателей на отдельно выделенные строки не является одним непрерывным буфером. При некратном объёме подходят Scatterv/Gatherv или дополнение. Здесь A дополняется нулевыми строками до ceil(n/P)*P; после Gather выводятся только исходные n строк. Работают также P>n и пустые полезные диапазоны.

## Вывод

Коллективные и точечные варианты дали одинаковые частоты букв, π и произведения матриц. Проверены длина 100, некратные блоки, P>n и согласованное завершение при ошибках. На малых данных коммуникационные расходы могут превосходить вычисления; конкретное сравнение времени приведено в таблице.

## Источники

- [Лабораторная работа №6.pdf](<Лабораторная работа №6.pdf>).
- [Типовые контрольные вопросы №6.docx](<Типовые контрольные вопросы №6.docx>): исходные контрольные вопросы.
- [Спецификации OpenMP](https://www.openmp.org/specifications/) и [документация Open MPI](https://docs.open-mpi.org/en/v5.0.x/). Ответы учитывают используемую среду macOS.
