cmake_minimum_required(VERSION 3.9.1)

set(ProjectName BoidSimulation)
project(${ProjectName})

#//////////// C++ configuration //////////////

# Here we set the C++ version.
set(CMAKE_CXX_STANDARD 14)
# Here we enable all the warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#//////////// Lib configuration //////////////
find_library(SDL SDL2 PATHS "${CMAKE_SOURCE_DIR}/lib/SDL2")

#//////////// Executable configuration ///////

# Here we set where the binary files will be generated.
# The CMAKE_BINARY_DIR is the path to the top level of the build tree.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_executable(BoidSimulation.out "${CMAKE_SOURCE_DIR}/src/main.cpp")
