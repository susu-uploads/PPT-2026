cmake_minimum_required(VERSION 3.20)
project(PPTPractice3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

add_executable(task-6 "omp-loop-scheduling.cpp")
target_compile_options(task-6 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-6 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task-8 "omp-matrix-multiply.cpp")
target_compile_options(task-8 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-8 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task-9 "omp-sections.cpp")
target_compile_options(task-9 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-9 PRIVATE OpenMP::OpenMP_CXX)
