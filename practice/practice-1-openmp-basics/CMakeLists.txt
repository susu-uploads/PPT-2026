cmake_minimum_required(VERSION 3.20)
project(PPTPractice1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED COMPONENTS CXX)

add_executable(task-2 "task-2.cpp")
target_compile_options(task-2 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-2 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task-3 "task-3.cpp")
target_compile_options(task-3 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-3 PRIVATE OpenMP::OpenMP_CXX)
