#include "dijkstra.h"
#include "debug.h"

#include <queue>
#include <algorithm>
#include <iostream>

namespace dijkstra {

// ========== Graph ==========

Graph::Graph(int n) {
    adj_.resize(n);
    nodes_.resize(n);
    for (int i = 0; i < n; ++i) {
        nodes_[i] = i;
        idToIndex_[i] = i;
    }
}

/**
 * @brief 根据节点外部ID查找其内部数组索引
 * @param id 节点的外部标识ID
 * @return 对应的内部数组索引
 * @throws std::runtime_error 当指定ID的节点不存在时抛出异常
 */
int Graph::getIndex(int id) const {
    std::unordered_map<int, int>::const_iterator it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        throw std::runtime_error("Node " + std::to_string(id) + " does not exist");
    }
    return it->second;
}

/**
 * @brief 向图中添加一个节点
 * 
 * 如果指定 ID 的节点已存在，则直接返回，不做任何操作。
 * 新节点将被追加到邻接表末尾，并建立 ID 到内部索引的映射。
 * 
 * @param id 待添加节点的唯一标识符
 */
void Graph::addNode(int id) {
    if (idToIndex_.count(id)) return;  // 已存在
    int idx = static_cast<int>(adj_.size());
    idToIndex_[id] = idx;
    nodes_.push_back(id);
    adj_.emplace_back();
}

void Graph::addEdge(int from, int to, int64_t weight) {
    // 自动创建不存在的节点
    if (!idToIndex_.count(from)) addNode(from);
    if (!idToIndex_.count(to))   addNode(to);

    int u = getIndex(from);
    int v = getIndex(to);
    adj_[u].push_back({to, weight});
}

void Graph::addUndirectedEdge(int from, int to, int64_t weight) {
    addEdge(from, to, weight);
    addEdge(to, from, weight);
}

// ========== Dijkstra ==========

/**
 * @brief 执行 Dijkstra 最短路径算法，计算从源节点到图中所有其他节点的最短距离
 *
 * 使用小顶堆优化的贪心松弛策略，求解单源最短路径问题。
 * 时间复杂度 O((V+E) log V)。堆中可能存在过期数据，通过
 * `d > dist[u]` 判断跳过，保证每个节点仅被有效处理一次。
 *
 * @param graph 图的常引用，包含邻接表与节点信息
 * @param source 源节点的外部 ID
 * @return DijkstraResult 包含到各节点的最短距离数组 dist、前驱节点数组 prev 及源节点 ID
 * @throws std::runtime_error 当源节点在图中不存在时抛出异常
 */
DijkstraResult dijkstra(const Graph& graph, int source) {
    const std::vector<std::vector<Edge>>& adj = graph.adj();
    const std::vector<int>& nodes = graph.nodes();

    int n = static_cast<int>(adj.size());
    std::vector<int64_t> dist(n, INF);
    std::vector<int> prev(n, -1);

    // 找到源节点的内部索引
    int srcIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (nodes[i] == source) {
            srcIdx = i;
            break;
        }
    }
    if (srcIdx == -1) {
        throw std::runtime_error("Source node " + std::to_string(source) + " not found");
    }

    dist[srcIdx] = 0;

    // 小顶堆: (距离, 节点索引)
    using State = std::pair<int64_t, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, srcIdx);

#ifdef DIJKSTRA_DEBUG
    int iteration = 0;
    std::cerr << "[Dijkstra Debug] 源节点: " << source << " (内部索引 " << srcIdx << ")\n";
    std::cerr << "[Dijkstra Debug] 开始松弛迭代...\n";
#endif

    while (!pq.empty()) {
        State state = pq.top();   // State = std::pair<int64_t, int>
        int64_t d = state.first;
        int u = state.second;
        pq.pop();

        if (d > dist[u]){
            continue;  // 过期数据
        } 

#ifdef DIJKSTRA_DEBUG
        std::cerr << "[Dijkstra Debug] 迭代 #" << ++iteration
                  << " | 当前节点: " << nodes[u]
                  << " | 距离: " << d << "\n";
#endif

        for (const Edge& e : adj[u]) {
            int v = -1;
            // 在 nodes 中找 e.to 的索引
            for (int k = 0; k < n; ++k) {
                if (nodes[k] == e.to) { v = k; break; }
            }

            if (dist[v] > dist[u] + e.weight) {
                int64_t old = dist[v];
                dist[v] = dist[u] + e.weight;
                prev[v] = u;
                pq.emplace(dist[v], v);

#ifdef DIJKSTRA_DEBUG
                std::cerr << "    -> 松弛边 " << nodes[u] << " -> " << e.to
                          << " (权重=" << e.weight << ")"
                          << " | dist[" << e.to << "]: "
                          << (old == INF ? "INF" : std::to_string(old))
                          << " -> " << dist[v] << "\n";
#endif
            }
        }
    }

#ifdef DIJKSTRA_DEBUG
    std::cerr << "[Dijkstra Debug] 算法完成，共 " << iteration << " 次迭代\n";
#endif

    return DijkstraResult{std::move(dist), std::move(prev), source};
}

// ========== findPath: 起点→终点 最短路径 ==========

PathResult findPath(const Graph& graph, int start, int goal) {
    const std::vector<std::vector<Edge>>& adj = graph.adj();
    const std::vector<int>& nodes = graph.nodes();
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

    // 起点 = 终点，直接返回
    if (startIdx == goalIdx) {
        return PathResult{{start}, 0};
    }

    std::vector<int64_t> dist(n, INF);
    std::vector<int> prev(n, -1);
    dist[startIdx] = 0;

    using State = std::pair<int64_t, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    pq.emplace(0, startIdx);

#ifdef DIJKSTRA_DEBUG
    int iteration = 0;
    std::cerr << "[findPath Debug] start=" << start << " -> goal=" << goal << "\n";
#endif

    while (!pq.empty()) {
        State state = pq.top();   // State = std::pair<int64_t, int>
        int64_t d = state.first;
        int u = state.second;
        pq.pop();

        if (d > dist[u]) continue;

        // ★ 关键改造：找到目标节点后立即终止
        if (u == goalIdx) {
#ifdef DIJKSTRA_DEBUG
            std::cerr << "[findPath Debug] 到达目标! 迭代 #" << ++iteration
                      << " | 总代价: " << d << "\n";
#endif
            // 回溯路径
            std::vector<int> path;
            for (int at = goalIdx; at != -1; at = prev[at]) {
                path.push_back(nodes[at]);
            }
            std::reverse(path.begin(), path.end());
            return PathResult{std::move(path), d};
        }

#ifdef DIJKSTRA_DEBUG
        std::cerr << "[findPath Debug] 迭代 #" << ++iteration
                  << " | 节点: " << nodes[u] << " | dist=" << d << "\n";
#endif

        for (const Edge& e : adj[u]) {
            int v = -1;
            for (int k = 0; k < n; ++k) {
                if (nodes[k] == e.to) { v = k; break; }
            }

            if (dist[v] > dist[u] + e.weight) {
                int64_t old = dist[v];
                dist[v] = dist[u] + e.weight;
                prev[v] = u;
                pq.emplace(dist[v], v);

#ifdef DIJKSTRA_DEBUG
                std::cerr << "    -> 松弛 " << nodes[u] << " -> " << e.to
                          << " (w=" << e.weight << ")"
                          << " | dist[" << e.to << "]: "
                          << (old == INF ? "INF" : std::to_string(old))
                          << " -> " << dist[v] << "\n";
#endif
            }
        }
    }

    // 堆空了还没找到 goal → 不可达
#ifdef DIJKSTRA_DEBUG
    std::cerr << "[findPath Debug] 不可达: 堆已空，未找到目标节点\n";
#endif
    return PathResult{};
}

// ========== DijkstraResult ==========

std::vector<int> DijkstraResult::getPath(int target) const {
    // 在 nodes 中找 target 的索引
    int t = -1;
    for (size_t i = 0; i < dist.size(); ++i) {
        if (static_cast<int>(i) == target) { t = static_cast<int>(i); break; }
    }

    std::vector<int> path;
    if (static_cast<size_t>(target) >= dist.size() || dist[target] == INF) {
        return path;  // 不可达
    }

    for (int at = target; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace dijkstra
