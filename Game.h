// Game.h
#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "FogOfWar.h"
#include "PathFinder.h"  // 新增include
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

class Game {
private:
    std::vector<Map> maps;
    Player player;
    Map* currentMap;
    bool gameRunning;
    std::unique_ptr<FogOfWar> fogOfWar;
    bool fogModeEnabled;
    
    // 自动模式相关
    std::unique_ptr<PathFinder> pathFinder;
    std::vector<Position> currentPath;
    size_t currentPathIndex;
    bool autoModeEnabled;
    std::atomic<bool> autoModeRunning;
    std::thread autoModeThread;
    int autoMoveDelay;  // 自动移动延迟（毫秒）
    
public:
    Game();
    ~Game();  // 需要析构函数来管理线程
    
    void run();
    
private:
    void showMainMenu();
    void selectMap();
    void toggleFogMode();
    void toggleAutoMode();  // 新增：切换自动模式
    void playGame();
    void displayGameState() const;
    void displayMapWithFog() const;
    void showGameOver(bool won) const;
    
    // 自动模式功能
    void startAutoMode();
    void stopAutoMode();
    void autoModeWorker();
    bool calculatePath();
    void performAutoMove();
    void displayPath() const;
};

#endif