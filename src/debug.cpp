#include "debug.h"
#include <sstream>
#include <iomanip>

namespace debug {

std::string graphToString(const dijkstra::Graph& graph) {
    std::ostringstream oss;
    oss << "=== 图结构 ===\n";
    oss << "节点数: " << graph.nodeCount() << "\n\n";

    const auto& adj = graph.adj();
    const auto& nodes = graph.nodes();
    int n = static_cast<int>(adj.size());

    for (int i = 0; i < n; ++i) {
        oss << "节点 " << nodes[i] << " ->";
        if (adj[i].empty()) {
            oss << " (无出边)\n";
        } else {
            oss << "\n";
            for (const auto& e : adj[i]) {
                oss << "  +--[" << e.weight << "]--> " << e.to << "\n";
            }
        }
    }
    return oss.str();
}

std::string resultToString(const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << "=== Dijkstra 结果 ===\n";
    oss << "源点: " << result.source << "\n\n";

    const auto& dist = result.dist;
    const auto& prev = result.prev;

    if (!result.nodeNames) return oss.str();

    const auto& nodes = *result.nodeNames;

    for (size_t i = 0; i < dist.size(); ++i) {
        oss << "到 " << nodes[i] << " 的最短距离: ";
        if (dist[i] == dijkstra::INF) {
            oss << "不可达\n";
        } else {
            oss << dist[i];
            // 还原路径
            std::vector<std::string> path;
            for (int at = static_cast<int>(i); at != -1; at = prev[at]) {
                path.push_back(nodes[at]);
            }
            std::reverse(path.begin(), path.end());
            oss << "  路径: ";
            for (size_t j = 0; j < path.size(); ++j) {
                if (j > 0) oss << " -> ";
                oss << path[j];
            }
            oss << "\n";
        }
    }
    return oss.str();
}

std::string pathResultToString(const dijkstra::PathResult& result) {
    std::ostringstream oss;
    if (!result.reached()) {
        oss << "不可达\n";
        return oss.str();
    }

    oss << "总代价: " << result.totalCost << "\n";
    oss << "路径: ";
    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0) oss << " -> ";
        oss << result.path[i];
    }
    oss << "\n";
    return oss.str();
}

} // namespace debug
