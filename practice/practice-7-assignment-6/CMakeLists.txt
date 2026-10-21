cmake_minimum_required(VERSION 3.20)
project(PPTPractice6 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-17 "mpi-letter-count.cpp")
target_compile_options(task-17 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-17 PRIVATE MPI::MPI_CXX)

add_executable(task-18 "mpi-pi.cpp")
target_compile_options(task-18 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-18 PRIVATE MPI::MPI_CXX)

add_executable(task-19 "mpi-matrix-multiply.cpp")
target_compile_options(task-19 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-19 PRIVATE MPI::MPI_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  string(REPEAT "a" 100 long_word)
  foreach(p IN ITEMS 1 2 4)
    foreach(mode IN ITEMS collective point-to-point)
      set(flags)
      if(mode STREQUAL "point-to-point")
        set(flags --point-to-point)
      endif()
      foreach(word IN ITEMS a abracadabra "${long_word}")
        string(LENGTH "${word}" n)
        add_program_test(letters-n${n}-p${p}-${mode} task-17 INPUT "${n}\n${word}\n"
          PROCESSES ${p} ARGS ${flags} CHECK letters "${word}")
      endforeach()
      foreach(n IN ITEMS 1000000 1)
        add_program_test(pi-n${n}-p${p}-${mode} task-18 INPUT "${n}\n"
          PROCESSES ${p} ARGS ${flags} CHECK pi ${n})
      endforeach()
      foreach(n IN ITEMS 1 2 3 10)
        matrix_case(${n} input expected)
        add_program_test(matrix-n${n}-p${p}-${mode} task-19 INPUT "${input}"
          PROCESSES ${p} ARGS ${flags} CHECK matrix ${expected})
      endforeach()
      add_program_test(matrix-example-p${p}-${mode} task-19 INPUT "2\n1 3 4 8\n5 4 3 0\n"
        PROCESSES ${p} ARGS ${flags} CHECK matrix 14 4 44 16)
      add_program_test(invalid-word-length-p${p}-${mode} task-17 INPUT "3\nrvtabc\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужны длина 1..100 и строка этой длины из a..z.")
      add_program_test(empty-word-p${p}-${mode} task-17 INPUT "0\na\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужны длина 1..100 и строка этой длины из a..z.")
      add_program_test(uppercase-word-p${p}-${mode} task-17 INPUT "1\nA\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужны длина 1..100 и строка этой длины из a..z.")
      add_program_test(invalid-pi-size-p${p}-${mode} task-18 INPUT "0\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужно N > 0.")
      add_program_test(invalid-matrix-size-p${p}-${mode} task-19 INPUT "11\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужны размер 1..10 и две полные матрицы.")
      add_program_test(incomplete-matrix-p${p}-${mode} task-19 INPUT "2\n1 2\n"
        PROCESSES ${p} ARGS ${flags} EXIT_CODE 2 CHECK error "Нужны размер 1..10 и две полные матрицы.")
    endforeach()
  endforeach()
  foreach(mode IN ITEMS collective point-to-point)
    set(flags --benchmark)
    if(mode STREQUAL "point-to-point")
      list(APPEND flags --point-to-point)
    endif()
    add_program_test(timing-letters-${mode} task-17 INPUT "3\naba\n"
      PROCESSES 2 ARGS ${flags} CHECK timed-letters aba)
    add_program_test(timing-pi-${mode} task-18 INPUT "10000\n"
      PROCESSES 2 ARGS ${flags} CHECK timed-pi 10000)
    add_program_test(timing-matrix-${mode} task-19 INPUT "1\n2\n3\n"
      PROCESSES 2 ARGS ${flags} CHECK timed-matrix 6)
  endforeach()
endif()
