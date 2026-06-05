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
    std::string from;      // 起始节点名
    std::string to;        // 目标节点名
    std::string name;      // 边名称，格式 "from-to"，如 "A-B"
    int64_t weight;        // 边权重
};

// 图：邻接表表示
class Graph {
public:
    Graph() = default;

    // 添加节点
    void addNode(const std::string& id);

    // 添加有向边 from -> to, 权重 weight
    void addEdge(const std::string& from, const std::string& to, int64_t weight);

    // 添加无向边
    void addUndirectedEdge(const std::string& from, const std::string& to, int64_t weight);

    // 节点数量
    int nodeCount() const { return static_cast<int>(adj_.size()); }

    // 获取邻接表 (只读)
    const std::vector<std::vector<Edge>>& adj() const { return adj_; }

    // 获取节点 id 列表
    const std::vector<std::string>& nodes() const { return nodes_; }

private:
    // id -> 内部索引映射
    int getIndex(const std::string& id) const;

    std::unordered_map<std::string, int> idToIndex_;   // 节点名 → 数组下标
    std::vector<std::string> nodes_;                    // 内部索引 → 节点名
    std::vector<std::vector<Edge>> adj_;                // 邻接表
};

// Dijkstra 结果（源点到全图）
struct DijkstraResult {
    std::vector<int64_t> dist;    // 距离数组
    std::vector<int> prev;        // 前驱节点内部索引 (用于还原路径)
    std::string source;           // 源节点名
    const std::vector<std::string>* nodeNames;  // 节点名列表指针

    // 还原路径
    std::vector<std::string> getPath(const std::string& target) const;
};

// 单条路径规划结果（起点→终点模式）
struct PathResult {
    std::vector<std::string> path;   // 从 start 到 goal 的节点序列（空 = 不可达）
    int64_t totalCost;               // 总代价

    bool reached() const { return !path.empty(); }
};

// Dijkstra 算法（优先队列优化，O((V+E) log V)）
DijkstraResult dijkstra(const Graph& graph, const std::string& source);

// 起点→终点 最短路径规划
PathResult findPath(const Graph& graph, const std::string& start, const std::string& goal);

} // namespace dijkstra
