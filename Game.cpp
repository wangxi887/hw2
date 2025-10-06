// Game.cpp
#include "Game.h"
#include <iostream>
#include <conio.h>  // 用于_getch()，Windows系统
// Linux系统可以使用相应的终端输入函数

using namespace std;

Game::Game() : gameRunning(true), currentMap(nullptr) {
    // 初始化地图
    maps.push_back(Map::createMap1());
    maps.push_back(Map::createMap2());
}

void Game::run() {
    while (gameRunning) {
        showMainMenu();
    }
}

void Game::showMainMenu() {
    system("cls");  // Windows清屏
    // system("clear");  // Linux/Mac清屏
    
    cout << "====== 迷宫探险游戏 ======\n";
    cout << "1. 开始游戏\n";
    cout << "2. 选择地图\n";
    cout << "3. 退出游戏\n";
    cout << "请选择: ";
    
    char choice;
    cin >> choice;
    
    switch (choice) {
        case '1':
            if (currentMap == nullptr) {
                currentMap = &maps[0];  // 默认选择第一张地图
            }
            playGame();
            break;
        case '2':
            selectMap();
            break;
        case '3':
            gameRunning = false;
            cout << "感谢游玩！再见！\n";
            break;
        default:
            cout << "无效选择，请重新输入！\n";
            break;
    }
}

void Game::selectMap() {
    system("cls");
    cout << "====== 选择地图 ======\n";
    for (size_t i = 0; i < maps.size(); i++) {
        cout << i + 1 << ". " << maps[i].getName() << "\n";
    }
    cout << "请选择地图: ";
    
    int choice;
    cin >> choice;
    
    if (choice >= 1 && choice <= static_cast<int>(maps.size())) {
        currentMap = &maps[choice - 1];
        cout << "已选择: " << currentMap->getName() << "\n";
    } else {
        cout << "无效选择！\n";
    }
    system("pause");  // Windows暂停
    // 在Linux/Mac上可以使用其他方法
}

void Game::playGame() {
    if (currentMap == nullptr) return;
    
    // 初始化玩家位置
    Position startPos = currentMap->getStartPosition();
    player = Player(startPos.x, startPos.y, 100);
    player.setPosition(startPos);
    
    bool gameWon = false;
    
    while (player.isAlive() && !gameWon) {
        system("cls");
        displayGameState();
        
        cout << "使用 WASD 移动 (Q退出): ";
        char input;
        cin >> input;
        
        if (input == 'q' || input == 'Q') {
            return;
        }
        
        if (player.move(input, *currentMap)) {
            Position playerPos = player.getPosition();
            CellType currentCell = currentMap->getCell(playerPos.x, playerPos.y);
            
            // 检查陷阱
            if (currentCell == TRAP) {
                player.takeDamage(30);
                cout << "你踩中了陷阱！失去30点生命值！\n";
                // 陷阱消失
                const_cast<Map*>(currentMap)->setCell(playerPos.x, playerPos.y, EMPTY);
                system("pause");
            }
            
            // 检查是否到达终点
            if (currentCell == END) {
                gameWon = true;
            }
        }
    }
    
    showGameOver(gameWon);
    system("pause");
}

void Game::displayGameState() const {
    if (currentMap == nullptr) return;
    
    cout << "=== " << currentMap->getName() << " ===\n";
    cout << "生命值: " << player.getHealth() << "/" << player.getMaxHealth();
    cout << " (" << player.getHealthPercent() << "%)\n";
    cout << "步数: " << player.getSteps() << "\n\n";
    
    // 显示地图和玩家位置
    for (int y = 0; y < currentMap->getHeight(); y++) {
        for (int x = 0; x < currentMap->getWidth(); x++) {
            Position playerPos = player.getPosition();
            if (x == playerPos.x && y == playerPos.y) {
                cout << "P ";  // 玩家
            } else {
                CellType cell = currentMap->getCell(x, y);
                switch (cell) {
                    case EMPTY: cout << "  "; break;
                    case WALL: cout << "# "; break;
                    case TRAP: cout << "x "; break;
                    case START: cout << "S "; break;
                    case END: cout << "E "; break;
                    default: cout << "? "; break;
                }
            }
        }
        cout << endl;
    }
    cout << endl;
    cout << "图例: P=玩家, #=墙壁, x=陷阱, S=起点, E=终点\n";
    cout << "----------------------------------------\n";
}

void Game::showGameOver(bool won) const {
    system("cls");
    if (won) {
        cout << "🎉 恭喜！你成功走出了迷宫！\n";
    } else {
        cout << "💀 游戏结束！你的生命值已耗尽！\n";
    }
    cout << "总步数: " << player.getSteps() << "\n";
    cout << "剩余生命值: " << player.getHealth() << "\n";
}