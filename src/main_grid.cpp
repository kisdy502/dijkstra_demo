#include "grid_dijkstra.h"
#include "grid_astar.h"
#include "log.h"

#include <iostream>
#include <chrono>

int main()
{
    SET_LOG_LEVEL(logdetail::Level::INFO);

    std::cout << "========================================\n";
    std::cout << "  Dijkstra vs A* 对比\n";
    std::cout << "========================================\n\n";

    // ===== 构建栅格地图 =====
    //   6行 x 10列，障碍物约 1/4 (15个)
    grid::GridMap map;
    map.rows = 6;
    map.cols = 10;
    map.grid = {
        // 列: 0  1  2  3  4  5  6  7  8  9
        /*行0*/ {1, 1, 1,-1,-1, 1, 1, 1, 1, 1},
        /*行1*/ {1, 1, 1, 1, 1, 1,-1, 1, 1, 1},
        /*行2*/ {1,-1,-1, 1, 1, 1,-1, 1, 1, 1},
        /*行3*/ {1, 1, 1, 1,-1, 1,-1,-1, 1, 1},
        /*行4*/ {1, 1, 1, 1,-1, 1, 1, 1, 1, 1},
        /*行5*/ {1, 1,-1,-1,-1, 1, 1, 1, 1, 1},
    };

    std::cout << grid::mapToString(map) << "\n";

    // ================================================================
    //  测试: 同一张地图，同样起终点，对比 Dijkstra 和 A*
    // ================================================================

    grid::Point start{0, 0};
    grid::Point goal{5, 9};

    // ----- Dijkstra -----
    std::cout << "------------------ Dijkstra ------------------\n";
    std::chrono::steady_clock::time_point t_dijk_start = std::chrono::steady_clock::now();
    grid::GridPathResult result_dijk = grid::dijkstra(map, start, goal);
    std::chrono::steady_clock::time_point t_dijk_end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> dijk_elapsed = t_dijk_end - t_dijk_start;
    std::cout << "[Dijkstra 耗时] " << dijk_elapsed.count() << " ms\n\n";
    std::cout << grid::pathToString(map, result_dijk) << "\n";

    // ----- A* -----
    std::cout << "\n-------------------- A* ----------------------\n";
    std::chrono::steady_clock::time_point t_astar_start = std::chrono::steady_clock::now();
    grid::GridPathResult result_astar = grid::astar(map, start, goal);
    std::chrono::steady_clock::time_point t_astar_end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> astar_elapsed = t_astar_end - t_astar_start;
    std::cout << "[A* 耗时] " << astar_elapsed.count() << " ms\n\n";
    std::cout << grid::pathToString(map, result_astar) << "\n";

    // ----- 对比总结 -----
    std::cout << "==================== 对比 ====================\n";
    std::cout << "Dijkstra 总代价: " << result_dijk.totalCost
              << ", 路径步数: " << result_dijk.path.size() << "\n";
    std::cout << "A*       总代价: " << result_astar.totalCost
              << ", 路径步数: " << result_astar.path.size() << "\n";
    std::cout << "\n启发函数: 曼哈顿距离 (|dr| + |dc|)\n";
    std::cout << "结论: 代价相同 → A* 找到的是最优解\n";
    std::cout << "      A* 探索轮数更少 → 搜索更高效（朝目标方向前进）\n";

    return 0;
}
