cmake_minimum_required(VERSION 3.20)
project(PPTPractice4 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-11 "mpi-hello-world.cpp")
target_compile_options(task-11 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-11 PRIVATE MPI::MPI_CXX)

add_executable(task-12 "mpi-even-odd.cpp")
target_compile_options(task-12 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-12 PRIVATE MPI::MPI_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  foreach(p IN ITEMS 1 2 4)
    add_program_test(hello-world-p${p} task-11 PROCESSES ${p} CHECK hello ${p})
    add_program_test(even-odd-p${p} task-12 PROCESSES ${p} CHECK parity ${p})
  endforeach()
endif()
