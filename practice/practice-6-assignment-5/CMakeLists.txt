cmake_minimum_required(VERSION 3.20)
project(PPTPractice5 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(MPI REQUIRED COMPONENTS CXX)

add_executable(task-13 "mpi-relay.cpp")
target_compile_options(task-13 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-13 PRIVATE MPI::MPI_CXX)

add_executable(task-14 "mpi-master.cpp")
target_compile_options(task-14 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-14 PRIVATE MPI::MPI_CXX)

add_executable(task-15 "mpi-ring.cpp")
target_compile_options(task-15 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-15 PRIVATE MPI::MPI_CXX)

add_executable(task-16 "mpi-all-to-all.cpp")
target_compile_options(task-16 PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(task-16 PRIVATE MPI::MPI_CXX)

include("../../cmake/PracticeTests.cmake")
if(BUILD_TESTING)
  foreach(p IN ITEMS 1 2 4)
    add_program_test(relay-p${p} task-13 PROCESSES ${p} CHECK relay ${p})
    add_program_test(master-p${p} task-14 PROCESSES ${p} CHECK master ${p})
    add_program_test(ring-p${p} task-15 PROCESSES ${p} CHECK ring ${p})
    add_program_test(all-to-all-p${p} task-16 PROCESSES ${p} CHECK all-to-all ${p})
  endforeach()
endif()
