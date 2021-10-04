cmake_minimum_required(VERSION 3.9.1)

project(BoidSimulation)

#//////////// C++ configuration //////////////

# Here we set the C++ version.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Here we enable all the warnings.
# -Wno-dev is to not display warning for developers(e.g opengl). 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#//////////// SDL2 configuration(just for linux) //////////////

# When we use find_package and serach for SDL2 we'll get some
# variables like SDL2_LIBRARIES, SDL2_INCLUDE_DIRS and others
# by default.
# The "link_directories" add directories in which the linker
# will look for libraries.

if (UNIX)
   # SDL2
   find_package(SDL2 REQUIRED)
   include_directories(${SDL2_INCLUDE_DIR})
   link_directories(${SDL2_LIB_DIR})
   
   # OpenGL
   find_package(OpenGL REQUIRED)
   include_directories(${OPENGL_INCLUDE_DIR})
   link_directories(${OPENGL_LIB_DIR})
endif (UNIX)

set(SDL2_LIBRARIES SDL2main SDL2)
set(OPENGL_LIBRARIES GL GLU)

#//////////// Executable configuration ///////

# Here we set where the binary files will be generated.
# The CMAKE_BINARY_DIR is the path to the top level of the build tree.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")
add_executable(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Here we link the libraries.

# We could use ${SDL2_LIBRARIES} but that will include all the
# libraries which it would be a waste of memory and time.
if (UNIX)
   target_link_libraries(BoidSimulation ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
endif (UNIX)
