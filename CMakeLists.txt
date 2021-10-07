cmake_minimum_required(VERSION 3.9.1)

project(BoidSimulation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libs")

# SDL2
find_package(SDL2 REQUIRED)
set(SDL2_LIBRARIES SDL2main SDL2)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

# GLAD
set(GLAD_DIR "${PROJECT_LIBRARIES_DIR}/glad")
add_library(glad STATIC "${GLAD_DIR}/include/glad/glad.h" "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")


# Executable
add_executable(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC glad ${SDL2_LIBRARIES} PRIVATE ${CMAKE_DL_LIBS})
   # CMAKE_DL_LIBS -> is the library libdl which helps to link dynamic
   # libraries. We need it to use GLAD.
