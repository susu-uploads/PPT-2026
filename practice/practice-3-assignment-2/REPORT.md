# Практическая работа №2

Бабушкин Михаил Вадимович. Технологии параллельного программирования.

## Реализация

task-4 последовательно показывает shared, shared с задержкой 100 мс и private. Флаг --mode shared|shared-sleep|private позволяет запускать варианты отдельно; проверка исправленного варианта использует --mode private и не исполняет гонку. Задержка реализована через std::this_thread::sleep_for. Намеренная гонка сохранена только как учебная демонстрация. task-5 читает N и делит диапазон между двумя потоками через if; --extra читает k N и распределяет остаток между первыми потоками. Сумма хранится в long long, N ограничено 4294967295 для предотвращения переполнения.

Исходники находятся непосредственно в каталоге практики. Каждый независимый `.cpp` имеет собственную точку входа; вся практика собирается одной командой CMake. Зависимые варианты выбираются флагами.

## ИСХОДНЫЙ КОД

### omp-shared-private.cpp

```cpp
#include <chrono>
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <thread>

int main(int argc, char **argv) {
  // Режим all запускает все демонстрации; private позволяет проверить исправление отдельно.
  std::string mode = "all";
  if (argc == 3 && std::string(argv[1]) == "--mode")
    mode = argv[2];
  if ((argc != 1 && argc != 3) ||
      (argc == 3 && std::string(argv[1]) != "--mode") ||
      (mode != "all" && mode != "shared" && mode != "shared-sleep" &&
       mode != "private")) {
    std::cerr << "--mode all|shared|shared-sleep|private\n";
    return 2;
  }
  // Проверка ввода выполняется до создания команды потоков.
  int k;
  if (!(std::cin >> k) || k <= 0) {
    std::cerr << "Нужно k > 0.\n";
    return 2;
  }
  omp_set_dynamic(0);
  int rank = 0;
  // Общая rank намеренно содержит гонку данных: результат не гарантируется стандартом C++.
  if (mode == "all" || mode == "shared") {
    std::puts("Shared:");
#pragma omp parallel num_threads(k) shared(rank)
    {
      rank = omp_get_thread_num();
      std::printf("I am %d thread.\n", rank);
    }
  }
  // Задержка увеличивает окно конфликта, но не делает ошибочный результат детерминированным.
  if (mode == "all" || mode == "shared-sleep") {
    std::puts("Shared with sleep:");
#pragma omp parallel num_threads(k) shared(rank)
    {
      rank = omp_get_thread_num();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::printf("I am %d thread.\n", rank);
    }
  }
  // Каждый поток получает свою rank и присваивает ей значение до первого чтения.
  if (mode == "all" || mode == "private") {
    std::puts("Private:");
#pragma omp parallel num_threads(k) private(rank)
    {
      rank = omp_get_thread_num();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::printf("I am %d thread.\n", rank);
    }
  }
}
```

### omp-range-sum.cpp

```cpp
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>

int main(int argc, char **argv) {
  // В основном варианте вводится N, в дополнительном — k N.
  const bool extra = argc == 2 && std::string(argv[1]) == "--extra";
  // По условию основной вариант использует два потока.
  int k = 2;
  long long n;
  if ((argc != 1 && !extra) || (extra && !(std::cin >> k)) ||
      !(std::cin >> n) || k <= 0 || n < 0 || n > 4294967295LL) {
    std::cerr << "Ожидается N (0..4294967295), для --extra: k N, k > 0.\n";
    return 2;
  }
  omp_set_dynamic(0);
  long long sum = 0;
// reduction создаёт частные суммы и объединяет их при завершении области.
#pragma omp parallel num_threads(k) reduction(+ : sum)
  {
    const int rank = omp_get_thread_num(), size = omp_get_num_threads();
    long long start, end;
    // Оператор if делит основной диапазон между двумя потоками без пересечения.
    if (!extra && rank == 0) {
      start = 1;
      end = n / 2;
    } else if (!extra) {
      start = n / 2 + 1;
      end = n;
    } else {
      // При произвольном k остаток раздаётся первым потокам по одному элементу.
      const long long base = n / size, remainder = n % size;
      start = rank * base + (rank < remainder ? rank : remainder) + 1;
      end = start + base + (rank < remainder) - 1;
    }
    // Поток суммирует только принадлежащие ему числа; пустой диапазон даёт ноль.
    for (long long i = start; i <= end; ++i)
      sum += i;
    std::printf("[%d]: Sum = %lld\n", rank, sum);
  }
  // После окончания области sum уже содержит объединённый результат.
  std::printf("Sum = %lld\n", sum);
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
python3 verify.py
```

Число потоков для программ без собственного параметра задаёт `OMP_NUM_THREADS`; динамическое изменение размера команды отключено. MPI-программы запускаются через `mpirun --bind-to none -np P`. Порядок строк от разных исполнителей не фиксируется.

## Проверка

```text
OK: практика 2, 21 запусков
```

Проверки используют реальные исполняемые файлы и ограничение времени 30 с. Проверяется состав сообщений, численные результаты и ошибки входа, а не случайный порядок вывода. В демонстрации гонки №2 проверяется только исправленная private-часть; конкретный ошибочный shared-вывод не является критерием теста.

## Эксперименты


```bash
printf '4\n' | OMP_NUM_THREADS=4 .cache/build/task-4
```

```text
Shared:
I am 0 thread.
I am 2 thread.
I am 1 thread.
I am 3 thread.
Shared with sleep:
I am 1 thread.
I am 1 thread.
I am 1 thread.
I am 1 thread.
Private:
I am 3 thread.
I am 0 thread.
I am 2 thread.
I am 1 thread.
```

```bash
printf '5\n' | OMP_NUM_THREADS=4 .cache/build/task-5
```

```text
[0]: Sum = 3
[1]: Sum = 12
Sum = 15
```

```bash
printf '4 2\n' | OMP_NUM_THREADS=4 .cache/build/task-5 --extra
```

```text
[0]: Sum = 1
[3]: Sum = 0
[2]: Sum = 0
[1]: Sum = 2
Sum = 3
```

## Контрольные вопросы

### 1. Для чего нужны частные переменные? Не противоречит ли их существование реализуемой OpenMP модели программирования в общей памяти? Приведите содержательный пример частной переменной.

Частные переменные хранят промежуточное состояние каждого потока. Это совместимо с общей памятью: сама архитектура допускает доступ к общей памяти, но алгоритму нужны независимые копии. Пример — частная rank, содержащая номер потока, или аккумулятор частичной суммы.

### 2. Какие новые области видимости появляются в параллельной программе? Как они задаются?

Кроме лексических областей видимости языка появляется область совместного использования данных командой. shared означает один общий объект, private — отдельные неинициализированные копии, firstprivate — копии с начальным значением. reduction создаёт копии аккумулятора с последующим объединением. Доступность имени и совместность объекта — разные свойства.

### 3. Продемонстрируйте конфликт обращений к переменной rank в написанной программе? Всегда ли он возникает? Как его предотвратить?

После записи rank одним потоком другой поток может перезаписать её до printf. В демонстрации задержка увеличивает окно конфликта. Видимый сбой не обязан повторяться; несинхронизированный конфликт чтения и записи является гонкой данных и неопределённым поведением C++. Исправление — private(rank) или объявление rank внутри области. Нельзя считать отсутствие сбоя доказательством правильности общей версии.

### 4. Как оформляются в программе входные данные, промежуточные переменные и результаты выполнения параллельного региона?

Неизменяемые входные N и k доступны команде как общие данные. Локальные границы диапазона и счётчики принадлежат потоку. sum в reduction имеет частные копии внутри области и итоговый общий результат после объединения. Матрица результата может быть общей, если потоки записывают разные ячейки.

### 5. Какие дополнительные действия выполняет директива, если она имеет параметр reduction? Чем инициализируются частные переменные, создаваемые параметром reduction?

reduction создаёт для каждого участника частный аккумулятор, инициализирует его нейтральным элементом, затем объединяет копии указанной операцией. Для сложения это 0, для умножения 1. Исходный аккумулятор также участвует в итоговом объединении; здесь он заранее равен нулю.

## Вывод

В исправленной private-версии каждому потоку соответствует свой номер. Общая rank допускает конфликт, а задержка делает его заметнее, не создавая гарантии конкретного ошибочного вывода. Оба варианта суммирования дают N(N+1)/2, включая нулевой диапазон и k>N.

## Источники

- [Лабораторная работа №2.pdf](<Лабораторная работа №2.pdf>).
- [Типовые контрольные вопросы №2.docx](<Типовые контрольные вопросы №2.docx>): исходные контрольные вопросы.
- [Спецификации OpenMP](https://www.openmp.org/specifications/) и [документация Open MPI](https://docs.open-mpi.org/en/v5.0.x/). Ответы учитывают используемую среду macOS.
