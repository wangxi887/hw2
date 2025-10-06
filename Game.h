// Game.h
#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "FogOfWar.h"  // 新增include
#include <vector>
#include <memory>

class Game {
private:
    std::vector<Map> maps;
    Player player;
    Map* currentMap;
    bool gameRunning;
    std::unique_ptr<FogOfWar> fogOfWar;  // 使用智能指针管理迷雾
    bool fogModeEnabled;  // 是否启用迷雾模式
    
public:
    Game();
    
    void run();  // 运行游戏主循环
    
private:
    void showMainMenu();
    void selectMap();
    void toggleFogMode();  // 新增：切换迷雾模式
    void playGame();
    void displayGameState() const;
    void displayMapWithFog() const;  // 新增：带迷雾的地图显示
    void showGameOver(bool won) const;
};

#endif