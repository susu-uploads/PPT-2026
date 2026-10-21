cmake_minimum_required(VERSION 3.20)
project(PPTPractice2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

add_executable(task-4 "omp-shared-private.cpp")
target_compile_options(task-4 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-4 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task-5 "omp-range-sum.cpp")
target_compile_options(task-5 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-5 PRIVATE OpenMP::OpenMP_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  foreach(k IN ITEMS 1 2 4)
    add_program_test(private-k${k} task-4 INPUT "${k}\n" ARGS --mode private CHECK private ${k})
  endforeach()
  foreach(n IN ITEMS 0 1 5 10000)
    foreach(k IN ITEMS 2 4 8)
      add_program_test(extra-sum-n${n}-k${k} task-5 INPUT "${k} ${n}\n"
        ARGS --extra CHECK sum ${k} ${n} extra)
    endforeach()
    add_program_test(sum-n${n} task-5 INPUT "${n}\n" CHECK sum 2 ${n} base)
  endforeach()
  add_program_test(invalid-thread-count task-5 INPUT "0 5\n" ARGS --extra EXIT_CODE 2
    CHECK error "Ожидается N (0..4294967295), для --extra: k N, k > 0.")
  add_program_test(sum-overflow task-5 INPUT "4294967296\n" EXIT_CODE 2
    CHECK error "Ожидается N (0..4294967295), для --extra: k N, k > 0.")
endif()
