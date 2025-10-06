// PathFinder.cpp
#include "PathFinder.h"
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

vector<Position> PathFinder::findPath(const Position& start, const Position& end) {
    // 优先队列用于存储待探索节点
    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    
    // 用于记录每个位置的最佳成本
    unordered_map<int, unordered_map<int, int>> bestCosts;
    
    // 创建起始节点
    Node startNode(start, 0, heuristic(start, end));
    openSet.push(startNode);
    bestCosts[start.y][start.x] = 0;
    
    while (!openSet.empty()) {
        // 获取当前最佳节点
        Node current = openSet.top();
        openSet.pop();
        
        // 如果到达终点，重构路径
        if (current.pos == end) {
            return reconstructPath(&current);
        }
        
        // 检查所有相邻位置
        for (const Position& neighborPos : getNeighbors(current.pos)) {
            int newGCost = current.gCost + 1;  // 每步成本为1
            
            // 检查是否找到更优路径
            if (bestCosts[neighborPos.y].count(neighborPos.x) == 0 || 
                newGCost < bestCosts[neighborPos.y][neighborPos.x]) {
                
                bestCosts[neighborPos.y][neighborPos.x] = newGCost;
                int hCost = heuristic(neighborPos, end);
                
                Node neighborNode(neighborPos, newGCost, hCost, new Node(current));
                openSet.push(neighborNode);
            }
        }
    }
    
    // 没有找到路径
    return vector<Position>();
}

vector<Position> PathFinder::reconstructPath(Node* endNode) {
    vector<Position> path;
    Node* current = endNode;
    
    while (current != nullptr) {
        path.push_back(current->pos);
        current = current->parent;
    }
    
    reverse(path.begin(), path.end());
    return path;
}

int PathFinder::heuristic(const Position& a, const Position& b) {
    // 使用曼哈顿距离
    return abs(a.x - b.x) + abs(a.y - b.y);
}

vector<Position> PathFinder::getNeighbors(const Position& pos) {
    vector<Position> neighbors;
    
    // 四个方向的移动
    vector<Position> directions = {
        Position(0, -1),  // 上
        Position(1, 0),   // 右
        Position(0, 1),   // 下
        Position(-1, 0)   // 左
    };
    
    for (const Position& dir : directions) {
        Position newPos(pos.x + dir.x, pos.y + dir.y);
        
        // 检查位置是否有效且可通行
        if (currentMap->isValidPosition(newPos.x, newPos.y)) {
            CellType cellType = currentMap->getCell(newPos.x, newPos.y);
            if (cellType != WALL) {  // 墙壁不可通行
                neighbors.push_back(newPos);
            }
        }
    }
    
    return neighbors;
}

char PathFinder::getNextMove(const Position& current, const Position& target) {
    if (target.x > current.x) return 'd';  // 右
    if (target.x < current.x) return 'a';  // 左
    if (target.y > current.y) return 's';  // 下
    if (target.y < current.y) return 'w';  // 上
    return ' ';  // 相同位置
}