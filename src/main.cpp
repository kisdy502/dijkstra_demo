#include "dijkstra.h"
#include "debug.h"

#include <iostream>
#include <string>

int main() {
    std::cout << "========================================\n";
    std::cout << "  Dijkstra Algorithm Demo\n";
    std::cout << "========================================\n\n";

    // ===== 构建测试图 =====
    //
    //   示例图（无向图）：
    //
    //         (4)
    //    0 ------- 1
    //    | \       | \
    // (1)|  \(7) (2)|  \(3)
    //    |   \     |   \
    //    2 ------- 3 ------- 4
    //         (5)      (6)
    //
    //   预期：从节点 0 出发的最短路径：
    //   dist[0]=0, dist[1]=4, dist[2]=1, dist[3]=6, dist[4]=12
    //   路径 0->4: 0 -> 2 -> 3 -> 4 (距离 1+5+6=12)

    dijkstra::Graph graph;

    // 添加无向边
    graph.addUndirectedEdge(0, 1, 4);
    graph.addUndirectedEdge(0, 2, 1);
    graph.addUndirectedEdge(0, 3, 7);
    graph.addUndirectedEdge(1, 3, 2);
    graph.addUndirectedEdge(1, 4, 3);
    graph.addUndirectedEdge(2, 3, 5);
    graph.addUndirectedEdge(3, 4, 6);

    // ===== 打印图结构 =====
    std::cout << debug::graphToString(graph) << "\n";

    // ===== 运行 Dijkstra =====
    int source = 0;
    std::cout << "Running Dijkstra from source node " << source << "...\n\n";

    auto result = dijkstra::dijkstra(graph, source);

    // ===== 打印结果 =====
    std::cout << debug::resultToString(result) << "\n";

    // ===== 验证结果正确性 =====
    std::cout << debug::verifyResult(graph, result) << "\n";

    // ===== 额外测试：有向图 =====
    std::cout << "\n========================================\n";
    std::cout << "  Directed Graph Test\n";
    std::cout << "========================================\n\n";

    dijkstra::Graph digraph;
    digraph.addEdge(0, 1, 10);
    digraph.addEdge(0, 2, 3);
    digraph.addEdge(1, 2, 1);
    digraph.addEdge(2, 1, 4);
    digraph.addEdge(1, 3, 2);
    digraph.addEdge(2, 3, 8);
    digraph.addEdge(2, 4, 2);
    digraph.addEdge(3, 4, 7);
    digraph.addEdge(4, 3, 9);

    std::cout << debug::graphToString(digraph) << "\n";

    std::cout << "Running Dijkstra from source node 0...\n\n";
    auto result2 = dijkstra::dijkstra(digraph, 0);
    std::cout << debug::resultToString(result2) << "\n";
    std::cout << debug::verifyResult(digraph, result2) << "\n";

    // ===== findPath 测试：起点→终点 最短路径 =====
    std::cout << "\n========================================\n";
    std::cout << "  findPath Test (start -> goal)\n";
    std::cout << "========================================\n\n";

    // 在无向图上测试 0 → 4
    auto path1 = dijkstra::findPath(graph, 0, 4);
    std::cout << "Graph: 0 -> 4\n" << debug::pathResultToString(path1) << "\n";

    // 在有向图上测试 0 → 4
    auto path2 = dijkstra::findPath(digraph, 0, 4);
    std::cout << "Digraph: 0 -> 4\n" << debug::pathResultToString(path2) << "\n";

    // 测试不可达情况：有向图 4 → 0（没有反向边）
    std::cout << "Digraph: 4 -> 0 (no reverse edges)\n";
    auto path3 = dijkstra::findPath(digraph, 4, 0);
    std::cout << debug::pathResultToString(path3) << "\n";

    // 测试起点=终点
    std::cout << "Digraph: 2 -> 2 (same node)\n";
    auto path4 = dijkstra::findPath(digraph, 2, 2);
    std::cout << debug::pathResultToString(path4) << "\n";

    return 0;
}
