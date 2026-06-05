#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <limits>

namespace grid {

constexpr int64_t INF = std::numeric_limits<int64_t>::max();
constexpr int OBSTACLE = -1;  // 障碍物标记

// ========== 二维坐标 ==========
struct Point {
    int row, col;

    bool operator==(const Point& o) const { return row == o.row && col == o.col; }
    bool operator!=(const Point& o) const { return !(*this == o); }
};

// ========== 栅格地图 ==========
struct GridMap {
    int rows, cols;
    std::vector<std::vector<int>> grid;  // grid[r][c] = 通行代价（>0）或 OBSTACLE（-1）

    bool isWalkable(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] != OBSTACLE;
    }

    bool isWalkable(const Point& p) const {
        return isWalkable(p.row, p.col);
    }

    int cost(const Point& p) const {
        return grid[p.row][p.col];
    }
};

// ========== 结果 ==========
struct GridPathResult {
    std::vector<Point> path;  // 从 start 到 goal 的坐标序列（空 = 不可达）
    int64_t totalCost;

    bool reached() const { return !path.empty(); }
};

// ========== 栅格 Dijkstra（起点→终点）==========
// 返回从 start 到 goal 的最短路径（4 方向移动）
// 每步代价 = 目标格子的 cost 值
GridPathResult dijkstra(const GridMap& map, Point start, Point goal);

// ========== 可视化打印 ==========
// 打印栅格地图（用字符表示空地/障碍/路径）
std::string mapToString(const GridMap& map);
std::string pathToString(const GridMap& map, const GridPathResult& result);

}  // namespace grid
