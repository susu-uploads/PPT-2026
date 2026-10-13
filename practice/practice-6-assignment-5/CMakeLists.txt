cmake_minimum_required(VERSION 3.20)
project(PPTPractice5 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-13 "PPT-Практика-5-Задание-13-БабушкинМВ.cpp")
target_compile_options(task-13 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-13 PRIVATE MPI::MPI_CXX)

add_executable(task-14 "PPT-Практика-5-Задание-14-БабушкинМВ.cpp")
target_compile_options(task-14 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-14 PRIVATE MPI::MPI_CXX)

add_executable(task-15 "PPT-Практика-5-Задание-15-БабушкинМВ.cpp")
target_compile_options(task-15 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-15 PRIVATE MPI::MPI_CXX)

add_executable(task-16 "PPT-Практика-5-Задание-16-БабушкинМВ.cpp")
target_compile_options(task-16 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-16 PRIVATE MPI::MPI_CXX)
