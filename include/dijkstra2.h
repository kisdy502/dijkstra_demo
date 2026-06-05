#pragma once

#include "dijkstra.h"  // 复用 Graph、Edge、INF 等定义
#include <string>
#include <vector>

namespace dijkstra2 {

// 算法结果
struct Result {
    std::vector<int64_t> dist;    // 距离数组（内部索引 → 最短距离）
    std::vector<int> prev;        // 前驱数组（内部索引 → 前驱内部索引，-1 表示无）
    std::string source;           // 源节点名
    const std::vector<std::string>* nodeNames;  // 节点名列表

    // 还原路径
    std::vector<std::string> getPath(const std::string& target) const;
};

// ============================================================
//  你需要实现这个函数！
//  输入: graph (图), source (源节点名)
//  输出: Result，包含 dist / prev / source
//
//  提示:
//    1. 用 graph.adj()  获取邻接表
//    2. 用 graph.nodes() 获取节点名列表
//    3. 用 INF 表示无穷大
//    4. 用 std::priority_queue 做小顶堆
//    5. prev[i] = -1 表示节点 i 没有前驱
// ============================================================
Result dijkstra(const dijkstra::Graph& graph, const std::string& source);

} // namespace dijkstra2
