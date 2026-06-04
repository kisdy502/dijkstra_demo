#include "debug.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>

namespace debug {

static const int COL_WIDTH = 8;

std::string graphToString(const dijkstra::Graph& graph) {
    std::ostringstream oss;
    oss << "=== Graph Structure ===\n";
    oss << "Nodes: " << graph.nodeCount() << "\n\n";

    const auto& adj = graph.adj();
    const auto& nodes = graph.nodes();
    int n = static_cast<int>(adj.size());

    for (int i = 0; i < n; ++i) {
        oss << "Node " << nodes[i] << " ->";
        if (adj[i].empty()) {
            oss << " (no outgoing edges)\n";
        } else {
            oss << "\n";
            for (const auto& e : adj[i]) {
                oss << "  +--[" << e.weight << "]--> " << e.to << "\n";
            }
        }
    }
    return oss.str();
}

std::string distToString(const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << "=== Distance Table ===\n";
    oss << std::left << std::setw(COL_WIDTH) << "Node"
        << std::setw(COL_WIDTH + 4) << "Distance\n";
    oss << std::string(COL_WIDTH + COL_WIDTH + 4, '-') << "\n";

    const auto& dist = result.dist;
    for (size_t i = 0; i < dist.size(); ++i) {
        oss << std::setw(COL_WIDTH) << i;
        if (dist[i] == dijkstra::INF) {
            oss << std::setw(COL_WIDTH + 4) << "INF";
        } else {
            oss << std::setw(COL_WIDTH + 4) << dist[i];
        }
        oss << "\n";
    }
    return oss.str();
}

std::string pathToString(const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << "=== Predecessor Table ===\n";
    oss << std::left << std::setw(COL_WIDTH) << "Node"
        << std::setw(COL_WIDTH) << "Prev\n";
    oss << std::string(COL_WIDTH * 2, '-') << "\n";

    const auto& prev = result.prev;
    for (size_t i = 0; i < prev.size(); ++i) {
        oss << std::setw(COL_WIDTH) << i;
        if (prev[i] == -1) {
            oss << std::setw(COL_WIDTH) << "NIL";
        } else {
            oss << std::setw(COL_WIDTH) << prev[i];
        }
        oss << "\n";
    }
    return oss.str();
}

std::string allPathsToString(const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << "=== Shortest Paths from Node " << result.source << " ===\n";

    const auto& dist = result.dist;
    const auto& prev = result.prev;

    for (size_t target = 0; target < dist.size(); ++target) {
        oss << std::setw(COL_WIDTH) << result.source
            << " -> " << std::setw(COL_WIDTH) << target
            << " : ";

        if (dist[target] == dijkstra::INF) {
            oss << "unreachable\n";
            continue;
        }

        oss << "dist=" << std::setw(COL_WIDTH) << dist[target] << "  path: ";

        // 还原路径
        std::vector<int> path;
        for (int at = static_cast<int>(target); at != -1; at = prev[at]) {
            path.push_back(at);
        }
        std::reverse(path.begin(), path.end());

        for (size_t j = 0; j < path.size(); ++j) {
            if (j > 0) oss << " -> ";
            oss << path[j];
        }
        oss << "\n";
    }
    return oss.str();
}

std::string resultToString(const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << distToString(result) << "\n";
    oss << pathToString(result) << "\n";
    oss << allPathsToString(result);
    return oss.str();
}

std::string verifyResult(const dijkstra::Graph& graph,
                         const dijkstra::DijkstraResult& result) {
    std::ostringstream oss;
    oss << "=== Correctness Verification ===\n";

    const auto& adj = graph.adj();
    const auto& nodes = graph.nodes();
    const auto& dist = result.dist;
    int n = static_cast<int>(adj.size());
    bool valid = true;

    // 1. 源点距离为 0
    for (int i = 0; i < n; ++i) {
        if (nodes[i] == result.source) {
            if (dist[i] != 0) {
                oss << "[FAIL] Source node distance should be 0, got " << dist[i] << "\n";
                valid = false;
            } else {
                oss << "[PASS] Source node distance is 0\n";
            }
            break;
        }
    }

    // 2. 三角不等式: dist[v] <= dist[u] + w(u,v) for all edges
    oss << "\n--- Triangle Inequality Check ---\n";
    int violations = 0;
    for (int u = 0; u < n; ++u) {
        if (dist[u] == dijkstra::INF) continue;
        for (const auto& e : adj[u]) {
            // 找到 e.to 的内部索引
            int v = -1;
            for (int k = 0; k < n; ++k) {
                if (nodes[k] == e.to) { v = k; break; }
            }
            if (dist[v] > dist[u] + e.weight) {
                oss << "[VIOLATION] Edge " << nodes[u] << " -> " << e.to
                    << " | dist[" << e.to << "]="
                    << (dist[v] == dijkstra::INF ? "INF" : std::to_string(dist[v]))
                    << " > dist[" << nodes[u] << "]=" << dist[u]
                    << " + w=" << e.weight
                    << " = " << (dist[u] + e.weight) << "\n";
                ++violations;
                valid = false;
            }
        }
    }
    if (violations == 0) {
        oss << "[PASS] All triangle inequalities satisfied (0 violations)\n";
    } else {
        oss << "[FAIL] " << violations << " triangle inequality violation(s) found\n";
    }

    // 3. 不可达节点检查
    oss << "\n--- Reachability Check ---\n";
    int unreachable = 0;
    for (int i = 0; i < n; ++i) {
        if (dist[i] == dijkstra::INF) {
            oss << "  Node " << nodes[i] << " is unreachable from source\n";
            ++unreachable;
        }
    }
    if (unreachable == 0) {
        oss << "[INFO] All nodes are reachable from source\n";
    } else {
        oss << "[INFO] " << unreachable << " node(s) unreachable\n";
    }

    oss << "\n=== Overall: " << (valid ? "PASS" : "FAIL") << " ===\n";
    return oss.str();
}

std::string pathResultToString(const dijkstra::PathResult& result) {
    std::ostringstream oss;
    oss << "=== Path Planning Result ===\n";

    if (!result.reached()) {
        oss << "[FAIL] No path found (unreachable)\n";
        return oss.str();
    }

    oss << "Total cost: " << result.totalCost << "\n";
    oss << "Path length: " << result.path.size() << " nodes\n";
    oss << "Route: ";
    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0) oss << " -> ";
        oss << result.path[i];
    }
    oss << "\n";
    return oss.str();
}

} // namespace debug
