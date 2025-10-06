// Map.h
#ifndef MAP_H
#define MAP_H

#include "position.h"
#include <vector>
#include <string>


enum CellType {
    EMPTY = 0,      // 可通行
    WALL = 1,       // 墙壁
    TRAP = 2,       // 陷阱
    START = 3,      // 起点
    END = 4         // 终点
};

class Map {
private:
    std::vector<std::vector<int>> grid;
    int width, height;
    Position startPos;
    Position endPos;
    std::string mapName;

public:
    Map(int w, int h, const std::string& name = "Unnamed Map");
    
    // 地图操作
    void setCell(int x, int y, CellType type);
    CellType getCell(int x, int y) const;
    
    // 获取地图信息
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Position getStartPosition() const { return startPos; }
    Position getEndPosition() const { return endPos; }
    std::string getName() const { return mapName; }
    
    // 验证地图有效性
    bool isValidPosition(int x, int y) const;
    bool hasValidPath() const;  // 检查是否存在从起点到终点的路径
    
    // 显示地图
    void display() const;
    
    // 预设地图
    static Map createMap1();
    static Map createMap2();
};

#endif