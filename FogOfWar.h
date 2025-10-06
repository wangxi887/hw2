// FogOfWar.h
#ifndef FOGOFWAR_H
#define FOGOFWAR_H

#include "Position.h"
#include <vector>
#include <memory>

enum FogState {
    FOG_UNEXPLORED = 0,  // 未探索
    FOG_VISIBLE = 1,     // 当前可见
    FOG_EXPLORED = 2     // 已探索（永久可见）
};

class FogOfWar {
private:
    std::vector<std::vector<FogState>> fogGrid;
    int width, height;
    int visionRange;  // 视野范围
    
public:
    FogOfWar(int mapWidth, int mapHeight, int range = 2);
    
    // 更新视野
    void updateVisibility(const Position& playerPos);
    
    // 获取迷雾状态
    FogState getFogState(int x, int y) const;
    
    // 检查位置是否在视野内
    bool isVisible(int x, int y) const;
    
    // 重置迷雾（开始新游戏时）
    void reset();
    
    // 获取探索进度
    float getExploredPercent() const;
    
private:
    // 检查位置是否在视野范围内
    bool isInVisionRange(int x, int y, const Position& center) const;
};

#endif