#include "dijkstra2.h"
#include "debug.h"

#include <iostream>

int main() {
    std::cout << "========================================\n";
    std::cout << "  Dijkstra2 自实现测试\n";
    std::cout << "========================================\n\n";

    // ===== 构建测试图（和之前一样，多了 G、H）=====
    dijkstra::Graph graph;

    graph.addUndirectedEdge("A", "B", 4);
    graph.addUndirectedEdge("A", "C", 1);
    graph.addUndirectedEdge("A", "D", 7);
    graph.addUndirectedEdge("B", "D", 2);
    graph.addUndirectedEdge("B", "E", 3);
    graph.addUndirectedEdge("C", "D", 5);
    graph.addUndirectedEdge("D", "E", 6);
    graph.addUndirectedEdge("D", "G", 7);
    graph.addUndirectedEdge("E", "G", 4);
    graph.addUndirectedEdge("G", "H", 2);

    std::cout << debug::graphToString(graph) << "\n";

    // ===== 运行你的 Dijkstra2 =====
    std::string source = "C";
    auto result = dijkstra2::dijkstra(graph, source);

    // ===== 打印结果 =====
    std::cout << "\n=== 结果 ===\n";
    std::cout << "源点: " << result.source << "\n\n";

    const auto& nodes = *result.nodeNames;
    for (size_t i = 0; i < result.dist.size(); ++i) {
        std::cout << "到 " << nodes[i] << " 的最短距离: ";
        if (result.dist[i] == dijkstra::INF) {
            std::cout << "不可达\n";
        } else {
            std::cout << result.dist[i] << "  路径: ";
            auto path = result.getPath(nodes[i]);
            for (size_t j = 0; j < path.size(); ++j) {
                if (j > 0) std::cout << " -> ";
                std::cout << path[j];
            }
            std::cout << "\n";
        }
    }

    return 0;
}
