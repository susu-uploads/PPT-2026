cmake_minimum_required(VERSION 3.20)
project(PPTPractice7 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-31 "hybrid-hello-world.cpp")
target_compile_options(task-31 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-31 PRIVATE OpenMP::OpenMP_CXX MPI::MPI_CXX)

add_executable(task-32 "hybrid-pi.cpp")
target_compile_options(task-32 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-32 PRIVATE OpenMP::OpenMP_CXX MPI::MPI_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  foreach(p IN ITEMS 1 2 4)
    foreach(k IN ITEMS 1 2 4)
      add_program_test(hello-world-p${p}-k${k} task-31 INPUT "${k}\n"
        PROCESSES ${p} CHECK hello ${p} ${k})
      add_program_test(pi-p${p}-k${k} task-32 INPUT "1000000\n"
        PROCESSES ${p} THREADS ${k} CHECK pi ${p} ${k} 1000000)
    endforeach()
  endforeach()
  add_program_test(serial-pi task-32 INPUT "1000000\n" ARGS --mode serial CHECK pi 1 1 1000000)
  add_program_test(openmp-pi task-32 INPUT "1000000\n" ARGS --mode openmp CHECK pi 1 4 1000000)
  add_program_test(pi-empty-ranges task-32 INPUT "1\n" PROCESSES 4 THREADS 2 CHECK pi 4 2 1)
  add_program_test(invalid-thread-count task-31 INPUT "0\n" PROCESSES 2 EXIT_CODE 2
    CHECK error "Нужно k > 0.")
  add_program_test(invalid-pi-size task-32 INPUT "0\n" PROCESSES 2 EXIT_CODE 2
    CHECK error "Нужно N > 0.")
  add_program_test(invalid-openmp-process-count task-32 INPUT "10\n" PROCESSES 2
    ARGS --mode openmp EXIT_CODE 2 CHECK error "--mode serial|openmp|hybrid; serial/openmp требуют один процесс.")
  add_program_test(invalid-mode task-32 INPUT "10\n" PROCESSES 2
    ARGS --mode bad EXIT_CODE 2 CHECK error "--mode serial|openmp|hybrid; serial/openmp требуют один процесс.")
endif()
