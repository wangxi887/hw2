// FogOfWar.cpp
#include "FogOfWar.h"
#include <cmath>
#include <iostream>

using namespace std;

FogOfWar::FogOfWar(int mapWidth, int mapHeight, int range) 
    : width(mapWidth), height(mapHeight), visionRange(range) {
    reset();
}

void FogOfWar::reset() {
    fogGrid.resize(height, vector<FogState>(width, FOG_UNEXPLORED));
}

void FogOfWar::updateVisibility(const Position& playerPos) {
    // 先将所有当前可见区域标记为已探索
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (fogGrid[y][x] == FOG_VISIBLE) {
                fogGrid[y][x] = FOG_EXPLORED;
            }
        }
    }
    
    // 更新玩家周围的视野
    for (int y = max(0, playerPos.y - visionRange); 
         y <= min(height - 1, playerPos.y + visionRange); y++) {
        for (int x = max(0, playerPos.x - visionRange); 
             x <= min(width - 1, playerPos.x + visionRange); x++) {
            
            if (isInVisionRange(x, y, playerPos)) {
                if (fogGrid[y][x] == FOG_UNEXPLORED) {
                    fogGrid[y][x] = FOG_VISIBLE;
                } else if (fogGrid[y][x] == FOG_EXPLORED) {
                    fogGrid[y][x] = FOG_VISIBLE;  // 重新点亮已探索区域
                }
            }
        }
    }
}

bool FogOfWar::isInVisionRange(int x, int y, const Position& center) const {
    // 简单的圆形视野检测
    int dx = x - center.x;
    int dy = y - center.y;
    double distance = sqrt(dx * dx + dy * dy);
    return distance <= visionRange;
}

FogState FogOfWar::getFogState(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return fogGrid[y][x];
    }
    return FOG_UNEXPLORED;
}

bool FogOfWar::isVisible(int x, int y) const {
    FogState state = getFogState(x, y);
    return state == FOG_VISIBLE || state == FOG_EXPLORED;
}

float FogOfWar::getExploredPercent() const {
    int explored = 0;
    int total = width * height;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (fogGrid[y][x] != FOG_UNEXPLORED) {
                explored++;
            }
        }
    }
    
    return total > 0 ? (float)explored / total * 100.0f : 0.0f;
}