cmake_minimum_required(VERSION 3.9.1)

set(ProjectName BoidSimulation)
project(${ProjectName})

#//////////// C++ configuration //////////////

# Here we set the C++ version.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD REQUIRED ON)
# Here we enable all the warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#//////////// SDL2 configuration(just for linux) //////////////
# When we use find_package and serach for SDL2 we'll get some
# variables like SDL2_LIBRARIES, SDL2_INCLUDE_DIRS and others
# by default.

if (UNIX)
   find_package(SDL2 REQUIRED)
endif (UNIX)


#//////////// Executable configuration ///////

# Here we set where the binary files will be generated.
# The CMAKE_BINARY_DIR is the path to the top level of the build tree.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_executable(BoidSimulation "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Here we link the libraries.
target_link_libraries(BoidSimulation ${SDL2_LIBRARIES})