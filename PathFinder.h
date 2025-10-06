// PathFinder.h
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Map.h"
#include "Position.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

struct Node {
    Position pos;
    int gCost;  // 从起点到当前节点的成本
    int hCost;  // 到终点的预估成本
    int fCost() const { return gCost + hCost; }
    Node* parent;
    
    Node(Position p, int g = 0, int h = 0, Node* par = nullptr)
        : pos(p), gCost(g), hCost(h), parent(par) {}
    
    bool operator>(const Node& other) const {
        return fCost() > other.fCost();
    }
};

class PathFinder {
private:
    const Map* currentMap;
    
public:
    PathFinder(const Map* map) : currentMap(map) {}
    
    // A* 路径查找算法
    std::vector<Position> findPath(const Position& start, const Position& end);
    
    // 获取下一步移动方向
    char getNextMove(const Position& current, const Position& target);
    
private:
    // 计算启发式成本（曼哈顿距离）
    int heuristic(const Position& a, const Position& b);
    
    // 获取可移动的相邻位置
    std::vector<Position> getNeighbors(const Position& pos);
    
    // 重构路径
    std::vector<Position> reconstructPath(Node* endNode);
};

#endif