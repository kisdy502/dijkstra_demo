#include "dijkstra2.h"
#include <queue>
#include <iostream>
#include <iomanip>

namespace dijkstra2
{

    // ============================================================
    //  日志辅助函数（已实现，可直接使用）
    // ============================================================

    static void printState(const std::vector<std::string> &nodes,
                           const std::vector<int64_t> &dist,
                           const std::vector<int> &prev)
    {
        std::cout << "  Node   Cost   Prev\n";
        std::cout << "  ------------------\n";
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            std::cout << "  " << std::setw(4) << nodes[i] << "  ";
            if (dist[i] == dijkstra::INF)
            {
                std::cout << std::setw(5) << "INF";
            }
            else
            {
                std::cout << std::setw(5) << dist[i];
            }
            std::cout << "  ";
            if (prev[i] == -1)
            {
                std::cout << std::setw(4) << "NIL";
            }
            else
            {
                std::cout << std::setw(4) << nodes[prev[i]];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // ============================================================
    //  TODO: 在这里实现你的 Dijkstra 算法
    // ============================================================

    Result dijkstra(const dijkstra::Graph &graph, const std::string &source)
    {
        const std::vector<std::vector<dijkstra::Edge>> &adj = graph.adj();
        const std::vector<std::string> &nodes = graph.nodes();

        int n = static_cast<int>(adj.size());
        std::vector<int64_t> dist(n, dijkstra::INF);
        std::vector<int> prev(n, -1);
        // dist: 所有节点距离设为 INF
        // prev: 所有节点前驱设为 -1

        // 2. 找到源节点的内部索引
        // 遍历 nodes，找到等于 source 的那个
        int srcIdx = -1;
        for (int i = 0; i < n; i++)
        {
            if (nodes[i] == source)
            {
                srcIdx = i;
                break;
            }
        }

        // 3. 源节点距离设为 0
        dist[srcIdx] = 0;

        using State = std::pair<int64_t, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
        pq.emplace(0, srcIdx);
        // 4. 小顶堆: std::priority_queue<std::pair<int64_t, int>, ...>
        //    放入 (0, srcIdx)

        // 5. 主循环:
        //    while (堆非空) {
        //        a) 取出堆顶
        //        b) 惰性删除: 如果取出的距离 > dist[节点]，跳过
        //        c) 遍历该节点的所有出边
        //        d) 对每条边 (u → v, 权重 w):
        //           如果 dist[v] > dist[u] + w:
        //               更新 dist[v] = dist[u] + w
        //               更新 prev[v] = u
        //               将 (dist[v], v) 入堆
        //    }

        // TODO: 你的实现 ...
        while (!pq.empty())
        {
            State minState = pq.top();
            pq.pop();
            int64_t weight = minState.first;
            int nodeIndex = minState.second;

            if (weight > dist[nodeIndex])
            {
                continue;
            }

            for (const dijkstra::Edge &e : adj[nodeIndex])
            {
                std::cout << "Edge:" << e.name << ",weight=" << e.weight << std::endl;
                int neighborIndex = -1;
                for (int k = 0; k < n; k++)
                {
                    std::string nodeId = nodes[k];
                    if (nodeId == e.to)
                    {
                        neighborIndex = k;
                        break;
                    }
                }

                if (dist[neighborIndex] > dist[nodeIndex] + e.weight)
                {
                    std::cout << "dist[" << neighborIndex << "](" << dist[neighborIndex] << "),大于 dist[" << nodeIndex << "](" << dist[nodeIndex] << ") +e.weight(" << e.weight << ")=" << dist[nodeIndex] + e.weight << std::endl;
                    dist[neighborIndex] = dist[nodeIndex] + e.weight;
                    prev[neighborIndex] = nodeIndex;
                    pq.emplace(dist[neighborIndex], neighborIndex); // 加入最小堆
                }
            }
        }

        std::cout << "=== Dijkstra 完成 ===\n\n";

        return Result{std::move(dist), std::move(prev), source, &graph.nodes()};
        // // 临时：返回空结果，编译通过
        // return Result{
        //     std::vector<int64_t>(n, dijkstra::INF),
        //     std::vector<int>(n, -1),
        //     source,
        //     &graph.nodes()};
    }

    // ============================================================
    //  getPath: 根据 prev 链还原路径（已实现，可直接使用）
    // ============================================================

    std::vector<std::string> Result::getPath(const std::string &target) const
    {
        std::vector<std::string> path;

        // 找 target 的内部索引
        int t = -1;
        if (nodeNames)
        {
            for (size_t i = 0; i < nodeNames->size(); ++i)
            {
                if ((*nodeNames)[i] == target)
                {
                    t = static_cast<int>(i);
                    break;
                }
            }
        }
        if (t == -1 || dist[t] == dijkstra::INF)
            return path; // 不存在或不可达

        // 沿 prev 链回溯
        for (int at = t; at != -1; at = prev[at])
        {
            path.push_back((*nodeNames)[at]);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

} // namespace dijkstra2
