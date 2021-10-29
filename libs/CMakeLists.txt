cmake_minimum_required(VERSION 3.9.1)

project(BoidSimulation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libs")

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# GLAD(OpenGL)
set(GLAD_DIR "${PROJECT_LIBRARIES_DIR}/glad")
add_library(glad
   "${GLAD_DIR}/src/glad.c")
include_directories("${GLAD_DIR}/include")

# ImGui
set(IMGUI_DIR "${PROJECT_LIBRARIES_DIR}/imgui")
add_library(imgui
   # Main ImGUI files
   "${IMGUI_DIR}/imgui.cpp"
   "${IMGUI_DIR}/imgui_draw.cpp"
   "${IMGUI_DIR}/imgui_widgets.cpp"
   "${IMGUI_DIR}/imgui_tables.cpp"
   # SDL2 + OpenGL-specific files
   "${IMGUI_DIR}/backends/imgui_impl_sdl.cpp"
   "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp")
target_include_directories(imgui PUBLIC
   "${IMGUI_DIR}/"
   "${IMGUI_DIR}/backends/"
   # Make SDL2 available to ImGUI
   "${GLAD_DIR}/include/"
   "${SDL2_INCLUDE_DIRS}")
## Configure ImGui to use OpenGL through SLD2
target_compile_definitions(imgui PUBLIC
   IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL_opengl.h>)

# Executable
add_executable(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC
   glad 
   ${SDL2_LIBRARIES} 
   imgui 
   PRIVATE ${CMAKE_DL_LIBS})
   # CMAKE_DL_LIBS -> is the library libdl which helps to link dynamic
   # libraries. We need it in order to use GLAD.
