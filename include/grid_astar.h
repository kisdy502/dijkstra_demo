#pragma once

#include "grid_dijkstra.h"  // 复用 Point, GridMap, GridPathResult

namespace grid {

// ========== 栅格 A*（起点→终点）==========
// 与 Dijkstra 的区别：
//   Dijkstra 堆排序键 = dist（只考虑已走距离）
//   A*      堆排序键 = dist + heuristic（已走距离 + 到终点的估算距离）
//
// 启发函数：曼哈顿距离 |dr| + |dc|
// 由于是 4 方向移动，曼哈顿距离是"可采纳的"（admissible），
// 保证 A* 找到的一定是最优解。
GridPathResult astar(const GridMap& map, Point start, Point goal);

}  // namespace grid
