// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"

class Player {
private:
    Position position;
    int health;
    int maxHealth;
    int stepsTaken;
    
public:
    Player(int startX = 0, int startY = 0, int maxHP = 100);
    
    // 移动操作
    bool move(char direction, const Map& map);
    
    // 生命值管理
    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const { return health > 0; }
    
    // 状态获取
    Position getPosition() const { return position; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getSteps() const { return stepsTaken; }
    float getHealthPercent() const { return (float)health / maxHealth * 100; }
    
    // 设置位置（用于初始化）
    void setPosition(const Position& pos) { position = pos; }
};

#endif