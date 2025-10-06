// Player.cpp
#include "Player.h"
#include "Map.h"
#include <iostream>

using namespace std;

Player::Player(int startX, int startY, int maxHP) 
    : position(startX, startY), maxHealth(maxHP), health(maxHP), stepsTaken(0) {}

bool Player::move(char direction, const Map& map) {
    int newX = position.x;
    int newY = position.y;
    
    switch (direction) {
        case 'w': case 'W': newY--; break;
        case 's': case 'S': newY++; break;
        case 'a': case 'A': newX--; break;
        case 'd': case 'D': newX++; break;
        default: return false;  // 无效输入
    }
    
    // 检查新位置是否可通行
    if (map.isValidPosition(newX, newY)) {
        CellType targetCell = map.getCell(newX, newY);
        if (targetCell != WALL) {
            position.x = newX;
            position.y = newY;
            stepsTaken++;
            return true;
        }
    }
    return false;
}

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}