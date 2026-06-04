#include "dijkstra.h"
#include "debug.h"

#include <iostream>

int main() {
    std::cout << "========================================\n";
    std::cout << "  Dijkstra 算法演示\n";
    std::cout << "========================================\n\n";

    // ===== 构建测试图（无向图）=====
    //
    //         (4)
    //     A ------- B
    //     | \       | \
    //   (1)|  \(7) (2)|  \(3)
    //     |   \     |   \
    //     C ------- D ------- E
    //         (5)       (6)
    //
    //   预期从 C 出发的最短路径：
    //   C=0, A=1, D=5, B=6, E=11

    dijkstra::Graph graph;

    graph.addUndirectedEdge("A", "B", 4);
    graph.addUndirectedEdge("A", "C", 1);
    graph.addUndirectedEdge("A", "D", 7);
    graph.addUndirectedEdge("B", "D", 2);
    graph.addUndirectedEdge("B", "E", 3);
    graph.addUndirectedEdge("C", "D", 5);
    graph.addUndirectedEdge("D", "E", 6);

    std::cout << debug::graphToString(graph) << "\n";

    // ===== 运行 Dijkstra（全图）=====
    std::string source = "C";
    auto result = dijkstra::dijkstra(graph, source);

    std::cout << debug::resultToString(result) << "\n\n";
    return 0;
    // ===== findPath 测试 =====
    std::cout << "========================================\n";
    std::cout << "  findPath 测试\n";
    std::cout << "========================================\n\n";

    auto path1 = dijkstra::findPath(graph, "A", "E");
    std::cout << "A -> E: " << debug::pathResultToString(path1) << "\n";

    auto path2 = dijkstra::findPath(graph, "C", "E");
    std::cout << "C -> E: " << debug::pathResultToString(path2) << "\n";

    return 0;
}
