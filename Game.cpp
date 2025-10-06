// Game.cpp
#include "Game.h"
#include <iostream>
#include <limits>

#ifdef _WIN32
    #include <conio.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    
    // Linux/Mac下的_getch替代实现
    int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

using namespace std;

Game::Game() : gameRunning(true), currentMap(nullptr), fogModeEnabled(false) {
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
    system(CLEAR_SCREEN);
    
    cout << "====== 迷宫探险游戏 ======\n";
    cout << "1. 开始游戏\n";
    cout << "2. 选择地图\n";
    cout << "3. " << (fogModeEnabled ? "禁用" : "启用") << "迷雾模式";
    if (fogModeEnabled) cout << " [已启用]";
    cout << "\n";
    cout << "4. 退出游戏\n";
    cout << "请选择: ";
    
    char choice;
    cin >> choice;
    
    switch (choice) {
        case '1':
            if (currentMap == nullptr) {
                currentMap = &maps[0];
            }
            playGame();
            break;
        case '2':
            selectMap();
            break;
        case '3':
            toggleFogMode();
            break;
        case '4':
            gameRunning = false;
            cout << "感谢游玩！再见！\n";
            break;
        default:
            cout << "无效选择，请重新输入！\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            break;
    }
}

void Game::toggleFogMode() {
    fogModeEnabled = !fogModeEnabled;
    cout << "迷雾模式 " << (fogModeEnabled ? "已启用" : "已禁用") << "！\n";
    
    if (fogModeEnabled && currentMap != nullptr) {
        fogOfWar = make_unique<FogOfWar>(
            currentMap->getWidth(), 
            currentMap->getHeight(), 
            2  // 5x5视野范围（半径2）
        );
    } else {
        fogOfWar.reset();  // 禁用时释放迷雾对象
    }
    
    system("pause");
}

void Game::selectMap() {
    system(CLEAR_SCREEN);
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
        
        // 如果迷雾模式启用，重新初始化迷雾
        if (fogModeEnabled) {
            fogOfWar = make_unique<FogOfWar>(
                currentMap->getWidth(), 
                currentMap->getHeight(), 
                2
            );
        }
    } else {
        cout << "无效选择！\n";
    }
    system("pause");
}

void Game::playGame() {
    if (currentMap == nullptr) return;
    
    // 初始化玩家位置
    Position startPos = currentMap->getStartPosition();
    player = Player(startPos.x, startPos.y, 100);
    player.setPosition(startPos);
    
    // 初始化迷雾（如果启用）
    if (fogModeEnabled) {
        fogOfWar = make_unique<FogOfWar>(
            currentMap->getWidth(), 
            currentMap->getHeight(), 
            2  // 5x5视野
        );
        fogOfWar->updateVisibility(player.getPosition());
    }
    
    bool gameWon = false;
    
    while (player.isAlive() && !gameWon) {
        system(CLEAR_SCREEN);
        displayGameState();
        
        cout << "使用 WASD 移动 (Q退出): ";
        char input;
        #ifdef _WIN32
            input = _getch();
        #else
            input = getch();
        #endif
        
        if (input == 'q' || input == 'Q') {
            return;
        }
        
        if (player.move(input, *currentMap)) {
            Position playerPos = player.getPosition();
            CellType currentCell = currentMap->getCell(playerPos.x, playerPos.y);
            
            // 更新迷雾视野
            if (fogModeEnabled) {
                fogOfWar->updateVisibility(playerPos);
            }
            
            // 检查陷阱
            if (currentCell == TRAP) {
                player.takeDamage(30);
                cout << "你踩中了陷阱！失去30点生命值！\n";
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
    cout << "步数: " << player.getSteps() << "\n";
    
    if (fogModeEnabled && fogOfWar) {
        cout << "探索进度: " << fogOfWar->getExploredPercent() << "%\n";
    }
    cout << "\n";
    
    // 根据是否启用迷雾模式选择显示方式
    if (fogModeEnabled && fogOfWar) {
        displayMapWithFog();
    } else {
        // 原来的显示逻辑
        for (int y = 0; y < currentMap->getHeight(); y++) {
            for (int x = 0; x < currentMap->getWidth(); x++) {
                Position playerPos = player.getPosition();
                if (x == playerPos.x && y == playerPos.y) {
                    cout << "P ";
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
    }
    cout << "----------------------------------------\n";
}

void Game::displayMapWithFog() const {
    Position playerPos = player.getPosition();
    
    for (int y = 0; y < currentMap->getHeight(); y++) {
        for (int x = 0; x < currentMap->getWidth(); x++) {
            FogState fogState = fogOfWar->getFogState(x, y);
            
            if (fogState == FOG_UNEXPLORED) {
                cout << "? ";  // 未探索区域
                continue;
            }
            
            // 可见或已探索区域
            if (x == playerPos.x && y == playerPos.y) {
                cout << "P ";
            } else {
                CellType cell = currentMap->getCell(x, y);
                switch (cell) {
                    case EMPTY: 
                        cout << "  "; 
                        break;
                    case WALL: 
                        cout << "# "; 
                        break;
                    case TRAP: 
                        // 已探索但当前不可见的陷阱显示为普通地面
                        if (fogState == FOG_VISIBLE) {
                            cout << "x ";
                        } else {
                            cout << "  ";
                        }
                        break;
                    case START: 
                        cout << "S "; 
                        break;
                    case END: 
                        // 已探索但当前不可见的终点显示为普通地面
                        if (fogState == FOG_VISIBLE) {
                            cout << "E ";
                        } else {
                            cout << "  ";
                        }
                        break;
                    default: 
                        cout << "? "; 
                        break;
                }
            }
        }
        cout << endl;
    }
    cout << endl;
    cout << "图例: P=玩家, #=墙壁, x=陷阱, S=起点, E=终点, ?=未探索区域\n";
    cout << "视野范围: 5x5 (周围2格)\n";
}

void Game::showGameOver(bool won) const {
    system(CLEAR_SCREEN);
    if (won) {
        cout << "🎉 恭喜！你成功走出了迷宫！\n";
    } else {
        cout << "💀 游戏结束！你的生命值已耗尽！\n";
    }
    cout << "总步数: " << player.getSteps() << "\n";
    cout << "剩余生命值: " << player.getHealth() << "\n";
    
    if (fogModeEnabled && fogOfWar) {
        cout << "最终探索进度: " << fogOfWar->getExploredPercent() << "%\n";
    }
}