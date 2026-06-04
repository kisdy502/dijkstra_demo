#pragma once

#include "dijkstra.h"
#include <string>

namespace debug {

// 打印图的结构
std::string graphToString(const dijkstra::Graph& graph);

// 打印 Dijkstra 结果完整信息
std::string resultToString(const dijkstra::DijkstraResult& result);

// 打印单条路径规划结果（起点→终点）
std::string pathResultToString(const dijkstra::PathResult& result);

} // namespace debug
