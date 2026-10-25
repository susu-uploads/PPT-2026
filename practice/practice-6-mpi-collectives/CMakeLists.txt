cmake_minimum_required(VERSION 3.20)
project(PPTPractice6 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-17 "task-17.cpp")
target_compile_options(task-17 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-17 PRIVATE MPI::MPI_CXX)

add_executable(task-18 "task-18.cpp")
target_compile_options(task-18 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-18 PRIVATE MPI::MPI_CXX)

add_executable(task-19 "task-19.cpp")
target_compile_options(task-19 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-19 PRIVATE MPI::MPI_CXX)
