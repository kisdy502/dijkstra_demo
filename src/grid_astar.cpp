#include "grid_astar.h"
#include "log.h"

#include <queue>
#include <algorithm>
#include <cstdlib>  // abs()

namespace grid {

// ========== 曼哈顿距离（启发函数）==========
// 4 方向移动时，这是"可采纳启发式"（never overestimates）
// 保证了 A* 一定找到最优解
static int64_t manhattan(const Point& a, const Point& b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

// ========== 栅格 A* ==========
GridPathResult astar(const GridMap& map, Point start, Point goal) {
    int R = map.rows;
    int C = map.cols;

    if (!map.isWalkable(start)) {
        LOG_ERROR("起点 ({}, {}) 是障碍物", start.row, start.col);
        return GridPathResult{};
    }
    if (!map.isWalkable(goal)) {
        LOG_ERROR("终点 ({}, {}) 是障碍物", goal.row, goal.col);
        return GridPathResult{};
    }
    if (start == goal) {
        return GridPathResult{{start}, 0};
    }

    // 距离表: dist[r][c] = 从起点到 (r,c) 的最短已知距离
    std::vector<std::vector<int64_t>> dist(R, std::vector<int64_t>(C, INF));
    std::vector<std::vector<Point>> prev(R, std::vector<Point>(C, {-1, -1}));

    dist[start.row][start.col] = 0;

    LOG_INFO("=== 栅格 A* 开始: ({}, {}) -> ({}, {}) ===",
             start.row, start.col, goal.row, goal.col);
    LOG_INFO("地图大小: {} x {}", R, C);

    // ============================================================
    // ★★★ A* 与 Dijkstra 的唯一区别在此 ★★★
    //
    // Dijkstra 的堆排序键:  State = (dist,        row, col)
    // A*       的堆排序键:  State = (dist + f,     row, col)
    //                                ↑ 加上启发值
    //
    // 堆按第 0 个元素排序 → A* 会优先探索"离终点近"的方向
    // ============================================================
    using State = std::tuple<int64_t, int, int>;  // (f值, row, col)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    int64_t h_start = manhattan(start, goal);
    pq.emplace(0 + h_start, start.row, start.col);

    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    const char* dirNames[] = {"上", "下", "左", "右"};

    int round = 0;
    int relaxedCount = 0;

    while (!pq.empty()) {
        State cur = pq.top();
        pq.pop();
        int64_t fValue = std::get<0>(cur);   // dist + heuristic（仅用于排序）
        int r = std::get<1>(cur);
        int c = std::get<2>(cur);

        // 惰性删除：f 值不直接比较，仍用 dist 判断
        int64_t distance = dist[r][c];  // 这才是真正的已走距离
        if (fValue > distance + manhattan({r, c}, goal)) {
            continue;  // 过时的 f 值，跳过
        }

        ++round;
        LOG_DEBUG("[第 {} 轮] 取出 ({}, {}) dist={}, f={}",
                  round, r, c, distance, fValue);

        if (r == goal.row && c == goal.col) {
            LOG_INFO("到达目标 ({}, {})，总代价={}, 共 {} 轮, 松弛 {} 次",
                     r, c, distance, round, relaxedCount);
            break;
        }

        for (int d = 0; d < 4; ++d) {
            int nr = r + dr[d];
            int nc = c + dc[d];

            if (!map.isWalkable(nr, nc)) {
                LOG_TRACE("  ({},{}) 方向{} -> ({},{}) 不可通行",
                          r, c, dirNames[d], nr, nc);
                continue;
            }

            int64_t newDist = dist[r][c] + map.grid[nr][nc];

            if (newDist < dist[nr][nc]) {
                int64_t oldDist = dist[nr][nc];
                dist[nr][nc] = newDist;
                prev[nr][nc] = {r, c};

                // ★ A* 的关键：push 的是 (newDist + 启发值, nr, nc)
                int64_t h = manhattan({nr, nc}, goal);
                pq.emplace(newDist + h, nr, nc);
                ++relaxedCount;

                LOG_DEBUG("  松弛 ({},{}) -> ({},{}) | {} -> {}, h={}, f={}",
                          r, c, nr, nc,
                          (oldDist == INF ? "INF" : std::to_string(oldDist)),
                          newDist, h, newDist + h);
            } else {
                LOG_TRACE("  跳过 ({},{}) -> ({},{}) | {} <= {} + {} = {}",
                          r, c, nr, nc,
                          dist[nr][nc], dist[r][c], map.grid[nr][nc],
                          dist[r][c] + map.grid[nr][nc]);
            }
        }
    }

    if (dist[goal.row][goal.col] == INF) {
        LOG_WARN("不可达: ({}, {}) -> ({}, {})",
                 start.row, start.col, goal.row, goal.col);
        return GridPathResult{};
    }

    // 回溯路径（与 Dijkstra 完全相同）
    std::vector<Point> path;
    Point cur = goal;
    while (!(cur == start)) {
        path.push_back(cur);
        cur = prev[cur.row][cur.col];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    LOG_INFO("路径长度: {} 步, 总代价: {}", path.size(), dist[goal.row][goal.col]);

    return GridPathResult{std::move(path), dist[goal.row][goal.col]};
}

}  // namespace grid
