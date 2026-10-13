cmake_minimum_required(VERSION 3.20)
project(PPTPractice4 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-11 "PPT-Практика-4-Задание-11-БабушкинМВ.cpp")
target_compile_options(task-11 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-11 PRIVATE MPI::MPI_CXX)

add_executable(task-12 "PPT-Практика-4-Задание-12-БабушкинМВ.cpp")
target_compile_options(task-12 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-12 PRIVATE MPI::MPI_CXX)
