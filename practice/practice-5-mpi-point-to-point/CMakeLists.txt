cmake_minimum_required(VERSION 3.20)
project(PPTPractice5 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-13 "task-13.cpp")
target_compile_options(task-13 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-13 PRIVATE MPI::MPI_CXX)

add_executable(task-14 "task-14.cpp")
target_compile_options(task-14 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-14 PRIVATE MPI::MPI_CXX)

add_executable(task-15 "task-15.cpp")
target_compile_options(task-15 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-15 PRIVATE MPI::MPI_CXX)

add_executable(task-16 "task-16.cpp")
target_compile_options(task-16 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-16 PRIVATE MPI::MPI_CXX)
