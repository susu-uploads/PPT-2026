cmake_minimum_required(VERSION 3.20)
project(PPTPractice7 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-31 "PPT-Практика-7-Задание-31-БабушкинМВ.cpp")
target_compile_options(task-31 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-31 PRIVATE OpenMP::OpenMP_CXX MPI::MPI_CXX)

add_executable(task-32 "PPT-Практика-7-Задание-32-БабушкинМВ.cpp")
target_compile_options(task-32 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-32 PRIVATE OpenMP::OpenMP_CXX MPI::MPI_CXX)
