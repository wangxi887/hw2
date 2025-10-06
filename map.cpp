// Map.cpp
#include "Map.h"
#include <iostream>
#include <random>
#include <queue>

using namespace std;

Map::Map(int w, int h, const string& name) 
    : width(w), height(h), mapName(name) {
    grid.resize(height, vector<int>(width, EMPTY));
}

void Map::setCell(int x, int y, CellType type) {
    if (isValidPosition(x, y)) {
        grid[y][x] = type;
        if (type == START) {
            startPos = Position(x, y);
        } else if (type == END) {
            endPos = Position(x, y);
        }
    }
}

CellType Map::getCell(int x, int y) const {
    if (isValidPosition(x, y)) {
        return static_cast<CellType>(grid[y][x]);
    }
    return WALL;  // 无效位置视为墙壁
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Map::hasValidPath() const {
    // 使用BFS检查路径是否存在
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    queue<Position> q;
    
    q.push(startPos);
    visited[startPos.y][startPos.x] = true;
    
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    while (!q.empty()) {
        Position current = q.front();
        q.pop();
        
        if (current == endPos) {
            return true;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            
            if (isValidPosition(nx, ny) && !visited[ny][nx] && 
                getCell(nx, ny) != WALL) {
                visited[ny][nx] = true;
                q.push(Position(nx, ny));
            }
        }
    }
    
    return false;
}

void Map::display() const {
    cout << "=== " << mapName << " === (" << width << "x" << height << ")\n";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            CellType cell = getCell(x, y);
            switch (cell) {
                case EMPTY: cout << " "; break;
                case WALL: cout << "#"; break;
                case TRAP: cout << "x"; break;
                case START: cout << "S"; break;
                case END: cout << "E"; break;
                default: cout << "?"; break;
            }
            cout << " ";
        }
        cout << endl;
    }
}

// 预设地图1：15x10 简单迷宫
Map Map::createMap1() {
    Map map(15, 10, "Forest Maze");
    
    // 设置边界墙壁
    for (int i = 0; i < 15; i++) {
        map.setCell(i, 0, WALL);
        map.setCell(i, 9, WALL);
    }
    for (int i = 0; i < 10; i++) {
        map.setCell(0, i, WALL);
        map.setCell(14, i, WALL);
    }
    
    // 内部墙壁
    vector<Position> walls = {
        {2,1}, {2,2}, {2,3}, {2,4}, {2,5}, {2,6}, {2,7},
        {4,2}, {4,3}, {4,4}, {4,5}, {4,6}, {4,7}, {4,8},
        {6,1}, {6,2}, {6,3}, {6,4}, {6,5}, {6,6},
        {8,3}, {8,4}, {8,5}, {8,6}, {8,7}, {8,8},
        {10,1}, {10,2}, {10,3}, {10,4}, {10,5}, {10,6}, {10,7},
        {12,2}, {12,3}, {12,4}, {12,5}, {12,6}, {12,7}, {12,8}
    };
    
    for (const auto& wall : walls) {
        map.setCell(wall.x, wall.y, WALL);
    }
    
    // 陷阱
    map.setCell(3, 3, TRAP);
    map.setCell(7, 4, TRAP);
    map.setCell(11, 5, TRAP);
    map.setCell(5, 8, TRAP);
    
    // 起点和终点
    map.setCell(1, 1, START);
    map.setCell(13, 8, END);
    
    return map;
}

// 预设地图2：15x10 复杂迷宫
Map Map::createMap2() {
    Map map(15, 10, "Dungeon Challenge");
    
    // 设置边界墙壁
    for (int i = 0; i < 15; i++) {
        map.setCell(i, 0, WALL);
        map.setCell(i, 9, WALL);
    }
    for (int i = 0; i < 10; i++) {
        map.setCell(0, i, WALL);
        map.setCell(14, i, WALL);
    }
    
    // 更复杂的内部墙壁布局
    vector<Position> walls = {
        {2,1}, {2,2}, {2,3}, {2,4}, {2,5}, {2,6}, {2,7}, {2,8},
        {4,1}, {4,2}, {4,3}, {4,4}, {4,5}, {4,6},
        {6,2}, {6,3}, {6,4}, {6,5}, {6,6}, {6,7}, {6,8},
        {8,1}, {8,2}, {8,3}, {8,4}, {8,5}, {8,6},
        {10,1}, {10,2}, {10,3}, {10,4}, {10,5}, {10,6}, {10,7}, {10,8},
        {12,1}, {12,2}, {12,3}, {12,4}, {12,5}, {12,6}, {12,7}
    };
    
    for (const auto& wall : walls) {
        map.setCell(wall.x, wall.y, WALL);
    }
    
    // 更多陷阱
    map.setCell(3, 2, TRAP);
    map.setCell(5, 3, TRAP);
    map.setCell(7, 4, TRAP);
    map.setCell(9, 5, TRAP);
    map.setCell(11, 6, TRAP);
    map.setCell(3, 7, TRAP);
    map.setCell(13, 3, TRAP);
    
    // 起点和终点
    map.setCell(1, 8, START);
    map.setCell(13, 1, END);
    
    return map;
}