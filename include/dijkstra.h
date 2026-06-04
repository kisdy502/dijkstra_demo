#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <cstdint>

namespace dijkstra {

// 无穷大
constexpr int64_t INF = std::numeric_limits<int64_t>::max();

// 边结构
struct Edge {
    int to;          // 目标节点
    int64_t weight;  // 边权重
};

// 图：邻接表表示
class Graph {
public:
    Graph() = default;
    explicit Graph(int n);

    // 添加节点
    void addNode(int id);

    // 添加有向边 from -> to, 权重 weight
    void addEdge(int from, int to, int64_t weight);

    // 添加无向边
    void addUndirectedEdge(int from, int to, int64_t weight);

    // 节点数量
    int nodeCount() const { return static_cast<int>(adj_.size()); }

    // 获取邻接表 (只读)
    const std::vector<std::vector<Edge>>& adj() const { return adj_; }

    // 获取节点 id 列表
    const std::vector<int>& nodes() const { return nodes_; }

private:
    // id -> 内部索引映射
    int getIndex(int id) const;

    std::unordered_map<int, int> idToIndex_;       //节点 id → 数组下标
    std::vector<int> nodes_;                       // 内部索引 -> id
    std::vector<std::vector<Edge>> adj_;           // 邻接表
};

// Dijkstra 结果（源点到全图）
struct DijkstraResult {
    std::vector<int64_t> dist;    // 距离数组
    std::vector<int> prev;        // 前驱节点 (用于还原路径)
    int source;                   // 源节点

    // 还原路径
    std::vector<int> getPath(int target) const;
};

// 单条路径规划结果（类似 Nav2 的起点→终点模式）
struct PathResult {
    std::vector<int> path;        // 从 start 到 goal 的节点序列（空 = 不可达）
    int64_t totalCost;            // 总代价

    bool reached() const { return !path.empty(); }
};

// Dijkstra 算法（优先队列优化，O((V+E) log V)）
// 计算源点到全图所有节点的最短距离
DijkstraResult dijkstra(const Graph& graph, int source);

// 起点→终点 最短路径规划（类似 Nav2 的规划模式）
// 找到 goal 后立即终止，不遍历全图
PathResult findPath(const Graph& graph, int start, int goal);

} // namespace dijkstra
