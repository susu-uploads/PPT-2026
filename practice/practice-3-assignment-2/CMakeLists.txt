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
