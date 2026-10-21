cmake_minimum_required(VERSION 3.20)
project(PPTPractice1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

add_executable(task-2 "omp-hello-world.cpp")
target_compile_options(task-2 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-2 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task-3 "omp-thread-ids.cpp")
target_compile_options(task-3 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-3 PRIVATE OpenMP::OpenMP_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  add_program_test(hello-world task-2 CHECK hello)
  foreach(k IN ITEMS 1 2 4)
    add_program_test(thread-ids-k${k} task-3 INPUT "${k}\n" CHECK ids ${k} 1)
    add_program_test(even-thread-ids-k${k} task-3 INPUT "${k}\n" ARGS --even CHECK ids ${k} 2)
  endforeach()
  add_program_test(invalid-thread-count task-3 INPUT "0\n" EXIT_CODE 2
    CHECK error "Введите положительное число потоков; вариант: --even.")
endif()
