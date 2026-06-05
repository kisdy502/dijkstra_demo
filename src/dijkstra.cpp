#include "dijkstra.h"

#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace dijkstra {

// ========== Graph ==========

int Graph::getIndex(const std::string& id) const {
    std::unordered_map<std::string, int>::const_iterator it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        throw std::runtime_error("Node " + id + " does not exist");
    }
    return it->second;
}

void Graph::addNode(const std::string& id) {
    if (idToIndex_.count(id)) return;
    int idx = static_cast<int>(adj_.size());
    idToIndex_[id] = idx;
    nodes_.push_back(id);
    adj_.emplace_back();
}

void Graph::addEdge(const std::string& from, const std::string& to, int64_t weight) {
    if (!idToIndex_.count(from)) addNode(from);
    if (!idToIndex_.count(to))   addNode(to);

    int u = getIndex(from);
    adj_[u].push_back({from, to, from + "-" + to, weight});
}

void Graph::addUndirectedEdge(const std::string& from, const std::string& to, int64_t weight) {
    addEdge(from, to, weight);
    addEdge(to, from, weight);
}

// ========== 日志辅助 ==========

// 打印当前 dist 和 prev 表
static void printState(const std::vector<std::string>& nodes,
                       const std::vector<int64_t>& dist,
                       const std::vector<int>& prev) {
    std::cout << "  Node   Cost   Prev\n";
    std::cout << "  ------------------\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::cout << "  " << std::setw(4) << nodes[i] << "  ";
        if (dist[i] == INF) {
            std::cout << std::setw(5) << "INF";
        } else {
            std::cout << std::setw(5) << dist[i];
        }
        std::cout << "  ";
        if (prev[i] == -1) {
            std::cout << std::setw(4) << "NIL";
        } else {
            std::cout << std::setw(4) << nodes[prev[i]];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// ========== Dijkstra ==========

DijkstraResult dijkstra(const Graph& graph, const std::string& source) {
    const std::vector<std::vector<Edge>>& adj = graph.adj();
    const std::vector<std::string>& nodes = graph.nodes();

    int n = static_cast<int>(adj.size());
    std::vector<int64_t> dist(n, INF);
    std::vector<int> prev(n, -1);

    // 找到源节点的内部索引
    int srcIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (nodes[i] == source) { srcIdx = i; break; }
    }
    if (srcIdx == -1) {
        throw std::runtime_error("Source node " + source + " not found");
    }

    dist[srcIdx] = 0;

    std::cout << "=== Dijkstra 开始: 源点 = " << source << " ===\n\n";
    std::cout << "[初始状态]\n";
    printState(nodes, dist, prev);

    // 小顶堆: (距离, 节点索引)
    using State = std::pair<int64_t, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, srcIdx);

    int round = 0;
    while (!pq.empty()) {
        State state = pq.top();                    // State = pair<距离, 节点索引>
        int64_t distance = state.first;            // first  = 源点到该节点的当前最短距离
        int currentIdx   = state.second;           // second = 该节点在 adj_ 中的内部索引
        pq.pop();

        if (distance > dist[currentIdx]){
            continue;   // 惰性删除
        } 

        std::cout << "--- 第 " << ++round << " 轮: 取出 " << nodes[currentIdx]
                  << " (dist=" << distance << ") ---\n";

        // 遍历 currentIdx 的所有出边，尝试松弛邻居
        for (const Edge& e : adj[currentIdx]) {
            int neighborIdx = -1;
            for (int k = 0; k < n; ++k) {
                if (nodes[k] == e.to) { neighborIdx = k; break; }
            }

            // 松弛操作
            if (dist[neighborIdx] > dist[currentIdx] + e.weight) {
                int64_t old = dist[neighborIdx];
                dist[neighborIdx] = dist[currentIdx] + e.weight;
                prev[neighborIdx] = currentIdx;
                pq.emplace(dist[neighborIdx], neighborIdx);

                std::cout << "  松弛边 [" << e.name << "] (权重 " << e.weight << ")"
                          << " | " << nodes[neighborIdx] << ": "
                          << (old == INF ? "INF" : std::to_string(old))
                          << " -> " << dist[neighborIdx] << "\n";
                std::cout << "  更新后状态:\n";
                printState(nodes, dist, prev);
            } else {
                std::cout << "  跳过边 [" << e.name << "] | " << nodes[neighborIdx] << " 当前距离 "
                          << (dist[neighborIdx] == INF ? "INF" : std::to_string(dist[neighborIdx]))
                          << " <= " << dist[currentIdx] << " + " << e.weight
                          << " = " << (dist[currentIdx] + e.weight) << " (不更新)\n";
            }
        }
    }

    std::cout << "=== Dijkstra 完成 ===\n\n";

    return DijkstraResult{std::move(dist), std::move(prev), source, &graph.nodes()};
}

// ========== findPath: 起点→终点 最短路径 ==========

PathResult findPath(const Graph& graph, const std::string& start, const std::string& goal) {
    const std::vector<std::vector<Edge>>& adj = graph.adj();
    const std::vector<std::string>& nodes = graph.nodes();
    int n = static_cast<int>(adj.size());

    // 找到起点和终点的内部索引
    int startIdx = -1, goalIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (nodes[i] == start) startIdx = i;
        if (nodes[i] == goal)  goalIdx  = i;
        if (startIdx != -1 && goalIdx != -1) break;
    }
    if (startIdx == -1) throw std::runtime_error("Start node not found");
    if (goalIdx  == -1) throw std::runtime_error("Goal node not found");

    if (startIdx == goalIdx) {
        return PathResult{{start}, 0};
    }

    std::vector<int64_t> dist(n, INF);
    std::vector<int> prev(n, -1);
    dist[startIdx] = 0;

    std::cout << "=== findPath 开始: " << start << " -> " << goal << " ===\n\n";
    std::cout << "[初始状态]\n";
    printState(nodes, dist, prev);

    using State = std::pair<int64_t, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, startIdx);

    int round = 0;
    while (!pq.empty()) {
        State state = pq.top();                    // State = pair<距离, 节点索引>
        int64_t distance = state.first;            // first  = 起点到该节点的当前最短距离
        int currentIdx   = state.second;           // second = 该节点在 adj_ 中的内部索引
        pq.pop();

        if (distance > dist[currentIdx]) continue;

        // 到达目标
        if (currentIdx == goalIdx) {
            std::cout << "--- 第 " << ++round << " 轮: 到达目标 " << goal
                      << " (总代价 " << distance << ") ---\n";

            std::vector<std::string> path;
            for (int at = goalIdx; at != -1; at = prev[at]) {
                path.push_back(nodes[at]);
            }
            std::reverse(path.begin(), path.end());

            std::cout << "路径: ";
            for (size_t i = 0; i < path.size(); ++i) {
                if (i > 0) std::cout << " -> ";
                std::cout << path[i];
            }
            std::cout << "\n\n";
            return PathResult{std::move(path), distance};
        }

        std::cout << "--- 第 " << ++round << " 轮: 取出 " << nodes[currentIdx]
                  << " (dist=" << distance << ") ---\n";

        for (const Edge& e : adj[currentIdx]) {
            int neighborIdx = -1;
            for (int k = 0; k < n; ++k) {
                if (nodes[k] == e.to) { neighborIdx = k; break; }
            }

            if (dist[neighborIdx] > dist[currentIdx] + e.weight) {
                int64_t old = dist[neighborIdx];
                dist[neighborIdx] = dist[currentIdx] + e.weight;
                prev[neighborIdx] = currentIdx;
                pq.emplace(dist[neighborIdx], neighborIdx);

                std::cout << "  松弛边 [" << e.name << "] (权重 " << e.weight << ")"
                          << " | " << nodes[neighborIdx] << ": "
                          << (old == INF ? "INF" : std::to_string(old))
                          << " -> " << dist[neighborIdx] << "\n";
                std::cout << "  更新后状态:\n";
                printState(nodes, dist, prev);
            } else {
                std::cout << "  跳过边 [" << e.name << "] | " << nodes[neighborIdx] << " 当前距离 "
                          << (dist[neighborIdx] == INF ? "INF" : std::to_string(dist[neighborIdx]))
                          << " <= " << dist[currentIdx] << " + " << e.weight
                          << " = " << (dist[currentIdx] + e.weight) << " (不更新)\n";
            }
        }
    }

    std::cout << "=== findPath 完成: 不可达 ===\n\n";
    return PathResult{};
}

// ========== DijkstraResult::getPath ==========

std::vector<std::string> DijkstraResult::getPath(const std::string& target) const {
    std::vector<std::string> path;

    // 找 target 的内部索引
    int t = -1;
    if (nodeNames) {
        for (size_t i = 0; i < nodeNames->size(); ++i) {
            if ((*nodeNames)[i] == target) { t = static_cast<int>(i); break; }
        }
    }
    if (t == -1) return path;  // 不存在

    if (dist[t] == INF) return path;  // 不可达

    for (int at = t; at != -1; at = prev[at]) {
        path.push_back((*nodeNames)[at]);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace dijkstra
