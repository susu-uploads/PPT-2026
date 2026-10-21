# Практическая работа №5

Бабушкин Михаил Вадимович. Технологии параллельного программирования.

## Реализация

Созданы независимые task-13..task-16. По тексту условий используются только блокирующие обмены: Send/Recv для эстафеты и мастера, Sendrecv для кольца и полного обмена. В подробных указаниях описаны неблокирующие функции; реализация следует требованию блокирующих обменов из условия. Полный обмен выполнен циклическими шагами, поэтому не опирается на буферизацию MPI_Send. При одном процессе эстафета возвращает 0 без посылки самому себе; полный обмен не передаёт сообщений.

Исходники находятся непосредственно в каталоге практики. Каждый независимый `.cpp` имеет собственную точку входа; вся практика собирается одной командой CMake. Зависимые варианты выбираются флагами.

## ИСХОДНЫЙ КОД

### mpi-relay.cpp

```cpp
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int value = 0;
  // Отдельный случай исключает блокирующую отправку самому себе.
  if (size == 1) {
    std::printf("[0]: receive message '0'\n");
  // Процесс 0 первым отправляет 0, затем ждёт возврата эстафеты от последнего процесса.
  } else if (rank == 0) {
    MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Recv(&value, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    std::printf("[0]: receive message '%d'\n", value);
  } else {
    // Рабочий процесс сначала получает значение от предыдущего соседа.
    MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    std::printf("[%d]: receive message '%d'\n", rank, value);
    // Перед пересылкой полученное значение увеличивается на единицу.
    ++value;
    MPI_Send(&value, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
  }

  // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
  MPI_Finalize();
}
```

### mpi-master.cpp

```cpp
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Мастер принимает отдельное сообщение от каждого рабочего в порядке рангов.
  if (rank == 0) {
    for (int source = 1; source < size; ++source) {
      int value;
      MPI_Recv(&value, 1, MPI_INT, source, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      std::printf("receive message '%d' from %d\n", value, source);
    }
  } else
    // Каждый рабочий отправляет мастеру собственный номер.
    MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
  MPI_Finalize();
}
```

### mpi-ring.cpp

```cpp
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int value = -1;
  // Операция по модулю замыкает последний и первый процессы в кольцо.
  const int next = (rank + 1) % size, previous = (rank - 1 + size) % size;
  // Блокирующий парный обмен совмещает отправку и приём без зависимости от буферизации Send.
  MPI_Sendrecv(&rank, 1, MPI_INT, next, 0, &value, 1, MPI_INT, previous, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  std::printf("[%d]: receive message '%d'\n", rank, value);

  // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
  MPI_Finalize();
}
```

### mpi-all-to-all.cpp

```cpp
#include <cstdio>
#include <mpi.h>

int main(int argc, char **argv) {
  // Инициализация выполняется один раз в каждом уже запущенном MPI-процессе.
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // За P−1 шагов каждый процесс обменивается сообщениями со всеми остальными.
  for (int offset = 1; offset < size; ++offset) {
    // На одном шаге все отправляют по одинаковому циклическому сдвигу.
    const int destination = (rank + offset) % size;
    const int source = (rank - offset + size) % size;
    int value = -1;
    // Источник и адресат согласованы между процессами; ожидание всех отправителей не возникает.
    MPI_Sendrecv(&rank, 1, MPI_INT, destination, 0, &value, 1, MPI_INT, source,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::printf("[%d]: receive message '%d' from %d\n", rank, value, source);
  }

  // Все обмены завершены; процесс освобождает ресурсы MPI перед выходом.
  MPI_Finalize();
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
python3 verify.py
```

Число потоков для программ без собственного параметра задаёт `OMP_NUM_THREADS`; динамическое изменение размера команды отключено. MPI-программы запускаются через `mpirun --bind-to none -np P`. Порядок строк от разных исполнителей не фиксируется.

## Проверка

```text
OK: практика 5, 12 запусков
```

Проверки используют реальные исполняемые файлы и ограничение времени 30 с. Проверяется состав сообщений, численные результаты и ошибки входа, а не случайный порядок вывода. В демонстрации гонки №2 проверяется только исправленная private-часть; конкретный ошибочный shared-вывод не является критерием теста.

## Эксперименты


```bash
mpirun --bind-to none -np 4 .cache/build/task-13
```

```text
[1]: receive message '0'
[2]: receive message '1'
[3]: receive message '2'
[0]: receive message '3'
```

```bash
mpirun --bind-to none -np 4 .cache/build/task-14
```

```text
receive message '1' from 1
receive message '2' from 2
receive message '3' from 3
```

```bash
mpirun --bind-to none -np 4 .cache/build/task-15
```

```text
[3]: receive message '2'
[0]: receive message '3'
[2]: receive message '1'
[1]: receive message '0'
```

```bash
mpirun --bind-to none -np 4 .cache/build/task-16
```

```text
[0]: receive message '3' from 3
[3]: receive message '2' from 2
[3]: receive message '1' from 1
[3]: receive message '0' from 0
[2]: receive message '1' from 1
[2]: receive message '0' from 0
[2]: receive message '3' from 3
[1]: receive message '0' from 0
[1]: receive message '3' from 3
[1]: receive message '2' from 2
[0]: receive message '2' from 2
[0]: receive message '1' from 1
```

## Контрольные вопросы

### 1. Рассказать общую схему межпроцессного обмена (перечислить участников, указать порядок действий).

Эстафета: процесс 0 отправляет 0 процессу 1 и принимает результат от последнего; остальные принимают, увеличивают и пересылают значение. Мастер–рабочие: рабочие отправляют ранги мастеру 0. Кольцо: каждый отправляет следующему и принимает от предыдущего. Каждый каждому: на шаге s процесс r отправляет (r+s) mod P и принимает от (r−s+P) mod P.

### 2. Данные какого типа передают все функции MPI? Привести общий формат функции передачи сообщений. Какие объекты идентифицируются этими параметрами?

MPI передаёт элементы буфера по описанию MPI_Datatype, например MPI_INT или MPI_DOUBLE. Общие параметры посылки: адрес buf, count, datatype, dest, tag, comm; приём дополнительно возвращает status. dest/source обозначают ранг, tag — тип сообщения в контексте приложения, comm — контекст обмена. Произвольный C++-объект с указателями нельзя передавать как готовую структуру памяти без сериализации или подходящего типа.

### 3. Рассказать про реализованные в MPI классификации механизмов передачи сообщений.

Различают точечные и коллективные коммуникации, блокирующие и неблокирующие операции. Отдельно определены стандартный, синхронный, буферизованный и готовый режимы посылки. Блокирующая стандартная посылка не обязательно ждёт входа получателя: это зависит от буферизации, поэтому на неё нельзя опираться для предотвращения тупика.

### 4. Для каждой задачи пояснить, почему для этого использован именно этот вид функций, получен ли прирост эффективности за счет их использования?

В эстафете порядок Send/Recv передаёт зависимость между соседями; в схеме мастер–рабочие мастер принимает отдельное сообщение от каждого рабочего. В кольце и полном обмене MPI_Sendrecv объединяет блокирующие отправку и приём без взаимного ожидания всех отправителей. Эти упражнения проверяют схемы сообщений; измеренного превосходства по времени над другой реализацией здесь не заявляется.

### 5. Как избежать синхронизации в порядке приема сообщений без смены функции? Как в этом случае определить отправителя сообщения?

Для приёма очередного доступного сообщения MPI_Recv может использовать MPI_ANY_SOURCE. После приёма фактический отправитель определяется по status.MPI_SOURCE. Это устраняет обязательное ожидание конкретного ранга, сохраняя блокирующий приём. В основном упражнении мастер принимает ранги по порядку для прозрачности схемы.

## Вывод

Все четыре схемы дают ожидаемые значения при P=1,2,4 и завершаются за установленное время. Блокирующий Sendrecv позволяет реализовать встречные обмены без зависимости от внутренних MPI-буферов. Глобальный порядок строк не является частью результата.

## Источники

- [Лабораторная работа №5.pdf](<Лабораторная работа №5.pdf>).
- [Типовые контрольные вопросы №5.docx](<Типовые контрольные вопросы №5.docx>): исходные контрольные вопросы.
- [Спецификации OpenMP](https://www.openmp.org/specifications/) и [документация Open MPI](https://docs.open-mpi.org/en/v5.0.x/). Ответы учитывают используемую среду macOS.
