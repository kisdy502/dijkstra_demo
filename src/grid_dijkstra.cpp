#include "grid_dijkstra.h"
#include "log.h"

#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace grid
{

    // ========== 栅格 Dijkstra ==========

    GridPathResult dijkstra(const GridMap &map, Point start, Point goal)
    {
        int R = map.rows;
        int C = map.cols;

        // 起点或终点是障碍物，直接返回不可达
        if (!map.isWalkable(start))
        {
            LOG_ERROR("起点 ({}, {}) 是障碍物", start.row, start.col);
            return GridPathResult{};
        }
        if (!map.isWalkable(goal))
        {
            LOG_ERROR("终点 ({}, {}) 是障碍物", goal.row, goal.col);
            return GridPathResult{};
        }
        if (start == goal)
        {
            return GridPathResult{{start}, 0};
        }

        // 距离表: dist[r][c]
        std::vector<std::vector<int64_t>> dist(R, std::vector<int64_t>(C, INF));
        // 前驱表: prev[r][c] = {pr, pc}，{-1,-1} 表示无前驱
        std::vector<std::vector<Point>> prev(R, std::vector<Point>(C, {-1, -1}));

        dist[start.row][start.col] = 0;

        LOG_INFO("=== 栅格 Dijkstra 开始: ({}, {}) -> ({}, {}) ===",
                 start.row, start.col, goal.row, goal.col);
        LOG_INFO("地图大小: {} x {}, 起点代价={}, 终点代价={}",
                 R, C, map.cost(start), map.cost(goal));

        // 小顶堆: (距离, row, col)
        using State = std::tuple<int64_t, int, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
        pq.emplace(0, start.row, start.col);

        // 4 方向: 上 下 左 右
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};
        const char *dirNames[] = {"上", "下", "左", "右"};

        int round = 0;
        int relaxedCount = 0; // 松弛次数统计

        while (!pq.empty())
        {
            // 取出堆顶的 State 元组，再拆成三个变量
            State cur = pq.top();
            pq.pop();
            int64_t distance = std::get<0>(cur);
            int r = std::get<1>(cur);
            int c = std::get<2>(cur);

            // 惰性删除
            if (distance > dist[r][c])
            {
                continue;
            }

            ++round;
            LOG_DEBUG("[第 {} 轮] 取出 ({}, {}) dist={}", round, r, c, distance);

            // 到达目标，提前退出
            if (r == goal.row && c == goal.col)
            {
                LOG_INFO("到达目标 ({}, {})，总代价={}, 共 {} 轮, 松弛 {} 次",
                         r, c, distance, round, relaxedCount);
                break;
            }

            // 遍历 4 个方向
            for (int d = 0; d < 4; ++d)
            {
                int nr = r + dr[d];
                int nc = c + dc[d];

                if (!map.isWalkable(nr, nc))
                {
                    LOG_TRACE("  ({},{}) 方向{} -> ({},{}) 不可通行",
                              r, c, dirNames[d], nr, nc);
                    continue;
                }

                int64_t newDist = dist[r][c] + map.grid[nr][nc];

                if (newDist < dist[nr][nc])
                {
                    int64_t oldDist = dist[nr][nc];
                    dist[nr][nc] =  newDist;
                    prev[nr][nc] = {r, c};
                    pq.emplace(newDist, nr, nc);
                    ++relaxedCount;

                    LOG_DEBUG("  松弛 ({},{}) -> ({},{}) | {} -> {}",
                              r, c, nr, nc,
                              (oldDist == INF ? "INF" : std::to_string(oldDist)),
                              newDist);
                }
                else
                {
                    LOG_TRACE("  跳过 ({},{}) -> ({},{}) | {} <= {} + {} = {}",
                              r, c, nr, nc,
                              dist[nr][nc], dist[r][c], map.grid[nr][nc],
                              dist[r][c] + map.grid[nr][nc]);
                }
            }
        }

        // 不可达
        if (dist[goal.row][goal.col] == INF)
        {
            LOG_WARN("不可达: ({}, {}) -> ({}, {})",
                     start.row, start.col, goal.row, goal.col);
            return GridPathResult{};
        }

        // 回溯路径
        std::vector<Point> path;
        Point cur = goal;
        while (!(cur == start))
        {
            path.push_back(cur);
            cur = prev[cur.row][cur.col];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());

        LOG_INFO("路径长度: {} 步, 总代价: {}", path.size(), dist[goal.row][goal.col]);

        return GridPathResult{std::move(path), dist[goal.row][goal.col]};
    }

    // ========== 可视化 ==========

    std::string mapToString(const GridMap &map)
    {
        std::ostringstream oss;
        oss << "=== 栅格地图 (" << map.rows << "x" << map.cols << ") ===\n";
        oss << "图例: □ 空地(代价1)  ■ 障碍物  数字=其他代价\n\n";

        // 列号
        oss << "   ";
        for (int c = 0; c < map.cols; ++c)
        {
            oss << std::setw(2) << c << " ";
        }
        oss << "\n";

        for (int r = 0; r < map.rows; ++r)
        {
            oss << std::setw(2) << r << " ";
            for (int c = 0; c < map.cols; ++c)
            {
                int v = map.grid[r][c];
                if (v == OBSTACLE)
                {
                    oss << " ■ ";
                }
                else if (v == 1)
                {
                    oss << " □ ";
                }
                else
                {
                    oss << std::setw(2) << v << " ";
                }
            }
            oss << "\n";
        }
        return oss.str();
    }

    std::string pathToString(const GridMap &map, const GridPathResult &result)
    {
        if (!result.reached())
        {
            return "不可达\n";
        }

        // 构建路径点集，便于快速判断
        std::vector<std::vector<char>> display(map.rows, std::vector<char>(map.cols, ' '));
        for (const Point &p : result.path)
        {
            display[p.row][p.col] = '*';
        }
        // 起点和终点特殊标记
        if (!result.path.empty())
        {
            const Point &s = result.path.front();
            const Point &g = result.path.back();
            display[s.row][s.col] = 'S';
            display[g.row][g.col] = 'G';
        }

        std::ostringstream oss;
        oss << "=== 最短路径 (代价=" << result.totalCost
            << ", 步数=" << result.path.size() << ") ===\n";
        oss << "图例: S=起点  G=终点  *=路径  □=空地  ■=障碍\n\n";

        oss << "   ";
        for (int c = 0; c < map.cols; ++c)
        {
            oss << std::setw(2) << c << " ";
        }
        oss << "\n";

        for (int r = 0; r < map.rows; ++r)
        {
            oss << std::setw(2) << r << " ";
            for (int c = 0; c < map.cols; ++c)
            {
                if (display[r][c] != ' ')
                {
                    oss << " " << display[r][c] << " ";
                }
                else if (map.grid[r][c] == OBSTACLE)
                {
                    oss << " ■ ";
                }
                else
                {
                    oss << " □ ";
                }
            }
            oss << "\n";
        }

        // 打印路径坐标序列
        oss << "\n路径序列:\n";
        for (size_t i = 0; i < result.path.size(); ++i)
        {
            if (i > 0)
                oss << " -> ";
            oss << "(" << result.path[i].row << "," << result.path[i].col << ")";
        }
        oss << "\n";

        return oss.str();
    }

} // namespace grid
