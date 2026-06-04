cmake_minimum_required(VERSION 3.14)
project(dijkstra_demo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Debug build by default
set(CMAKE_BUILD_TYPE Debug)

# Debug macro
add_compile_definitions(DIJKSTRA_DEBUG=$<CONFIG:Debug>)

add_executable(dijkstra_demo
    src/main.cpp
    src/dijkstra.cpp
    src/debug.cpp
)

target_include_directories(dijkstra_demo PRIVATE include)
