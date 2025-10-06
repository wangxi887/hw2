// Game.h
#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include <vector>

class Game {
private:
    std::vector<Map> maps;
    Player player;
    Map* currentMap;
    bool gameRunning;
    
public:
    Game();
    
    void run();  // 运行游戏主循环
    
private:
    void showMainMenu();
    void selectMap();
    void playGame();
    void displayGameState() const;
    void showGameOver(bool won) const;
};

#endif