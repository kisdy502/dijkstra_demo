#pragma once

#include "dijkstra.h"
#include <string>

namespace debug {

// 打印图的结构
std::string graphToString(const dijkstra::Graph& graph);

// 打印 Dijkstra 结果：距离表
std::string distToString(const dijkstra::DijkstraResult& result);

// 打印 Dijkstra 结果：路径表
std::string pathToString(const dijkstra::DijkstraResult& result);

// 打印 Dijkstra 结果：从源点到每个节点的路径
std::string allPathsToString(const dijkstra::DijkstraResult& result);

// 打印 Dijkstra 结果完整信息
std::string resultToString(const dijkstra::DijkstraResult& result);

// 验证 Dijkstra 结果的正确性
// 检查三角不等式 dist[v] <= dist[u] + weight(u,v) 对所有边成立
std::string verifyResult(const dijkstra::Graph& graph, const dijkstra::DijkstraResult& result);

// 打印单条路径规划结果（起点→终点）
std::string pathResultToString(const dijkstra::PathResult& result);

} // namespace debug
