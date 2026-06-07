cmake_minimum_required(VERSION 3.14)
project(dijkstra_demo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Debug build by default
set(CMAKE_BUILD_TYPE Debug)

# ===== 原始 Dijkstra 演示 =====
add_executable(dijkstra_demo
    src/main.cpp
    src/dijkstra.cpp
    src/debug.cpp
)

# ===== 你的自实现 Dijkstra2 =====
add_executable(dijkstra2_demo
    src/main2.cpp
    src/dijkstra.cpp
    src/dijkstra2.cpp
    src/debug.cpp
)

# ===== 栅格 Dijkstra 演示 =====
add_executable(grid_dijkstra_demo
    src/main_grid.cpp
    src/grid_dijkstra.cpp
    src/grid_astar.cpp
)

# ===== 模板学习 Demo =====
add_executable(template_demo
    src/template_demo.cpp
)

# ===== include 路径 =====
target_include_directories(dijkstra_demo PRIVATE include)
target_include_directories(dijkstra2_demo PRIVATE include)
target_include_directories(grid_dijkstra_demo PRIVATE include)
target_include_directories(template_demo PRIVATE include)
