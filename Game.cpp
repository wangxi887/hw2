// Game.cpp
#include "Game.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
    #define SLEEP(ms) Sleep(ms)
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    #define SLEEP(ms) usleep(ms * 1000)
    
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
using namespace std::chrono;

Game::Game() : gameRunning(true), currentMap(nullptr), 
               fogModeEnabled(false), autoModeEnabled(false),
               currentPathIndex(0), autoMoveDelay(500) {
    maps.push_back(Map::createMap1());
    maps.push_back(Map::createMap2());
}

Game::~Game() {
    stopAutoMode();  // 确保线程安全退出
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
    cout << "4. " << (autoModeEnabled ? "禁用" : "启用") << "自动模式";
    if (autoModeEnabled) cout << " [已启用]";
    cout << "\n";
    cout << "5. 退出游戏\n";
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
            toggleAutoMode();
            break;
        case '5':
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

void Game::toggleAutoMode() {
    autoModeEnabled = !autoModeEnabled;
    cout << "自动模式 " << (autoModeEnabled ? "已启用" : "已禁用") << "！\n";
    
    if (autoModeEnabled && currentMap != nullptr) {
        pathFinder = make_unique<PathFinder>(currentMap);
    } else {
        stopAutoMode();
        pathFinder.reset();
    }
    
    system("pause");
}

void Game::toggleFogMode() {
    fogModeEnabled = !fogModeEnabled;
    cout << "迷雾模式 " << (fogModeEnabled ? "已启用" : "已禁用") << "！\n";
    
    if (fogModeEnabled && currentMap != nullptr) {
        fogOfWar = make_unique<FogOfWar>(
            currentMap->getWidth(), 
            currentMap->getHeight(), 
            2
        );
    } else {
        fogOfWar.reset();
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
        
        // 重新初始化相关系统
        if (fogModeEnabled) {
            fogOfWar = make_unique<FogOfWar>(
                currentMap->getWidth(), 
                currentMap->getHeight(), 
                2
            );
        }
        
        if (autoModeEnabled) {
            pathFinder = make_unique<PathFinder>(currentMap);
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
    
    // 初始化系统
    if (fogModeEnabled) {
        fogOfWar = make_unique<FogOfWar>(
            currentMap->getWidth(), 
            currentMap->getHeight(), 
            2
        );
        fogOfWar->updateVisibility(player.getPosition());
    }
    
    if (autoModeEnabled) {
        pathFinder = make_unique<PathFinder>(currentMap);
        stopAutoMode();  // 确保之前的自动模式已停止
        currentPath.clear();
        currentPathIndex = 0;
    }
    
    bool gameWon = false;
    
    while (player.isAlive() && !gameWon) {
        system(CLEAR_SCREEN);
        displayGameState();
        
        // 自动模式处理
        if (autoModeEnabled && !autoModeRunning) {
            cout << "按 SPACE 开始自动寻路，Q退出: ";
        } else if (!autoModeEnabled) {
            cout << "使用 WASD 移动 (Q退出): ";
        } else {
            cout << "自动模式运行中... 按 SPACE 停止，Q退出\n";
        }
        
        char input;
        #ifdef _WIN32
            input = _getch();
        #else
            input = getch();
        #endif
        
        if (input == 'q' || input == 'Q') {
            stopAutoMode();
            return;
        }
        
        if (autoModeEnabled) {
            if (input == ' ' && !autoModeRunning) {
                // 开始自动模式
                if (calculatePath()) {
                    startAutoMode();
                } else {
                    cout << "无法找到路径到终点！\n";
                    system("pause");
                }
            } else if (input == ' ' && autoModeRunning) {
                // 停止自动模式
                stopAutoMode();
            }
        } else {
            // 手动模式
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
        
        // 检查自动模式是否完成
        if (autoModeRunning) {
            Position playerPos = player.getPosition();
            CellType currentCell = currentMap->getCell(playerPos.x, playerPos.y);
            if (currentCell == END) {
                gameWon = true;
                stopAutoMode();
            }
        }
    }
    
    stopAutoMode();
    showGameOver(gameWon);
    system("pause");
}

bool Game::calculatePath() {
    if (!pathFinder) return false;
    
    Position start = player.getPosition();
    Position end = currentMap->getEndPosition();
    
    currentPath = pathFinder->findPath(start, end);
    currentPathIndex = 0;
    
    return !currentPath.empty();
}

void Game::startAutoMode() {
    if (currentPath.empty() || autoModeRunning) return;
    
    autoModeRunning = true;
    autoModeThread = thread(&Game::autoModeWorker, this);
}

void Game::stopAutoMode() {
    autoModeRunning = false;
    if (autoModeThread.joinable()) {
        autoModeThread.join();
    }
}

void Game::autoModeWorker() {
    while (autoModeRunning && currentPathIndex < currentPath.size()) {
        performAutoMove();
        SLEEP(autoMoveDelay);
        
        // 检查是否到达终点
        Position playerPos = player.getPosition();
        if (playerPos == currentMap->getEndPosition()) {
            break;
        }
    }
    autoModeRunning = false;
}

void Game::performAutoMove() {
    if (currentPathIndex >= currentPath.size()) {
        stopAutoMode();
        return;
    }
    
    // 获取下一个目标位置
    Position nextPos = currentPath[currentPathIndex];
    
    // 计算移动方向
    char direction = pathFinder->getNextMove(player.getPosition(), nextPos);
    
    // 执行移动
    if (player.move(direction, *currentMap)) {
        Position playerPos = player.getPosition();
        CellType currentCell = currentMap->getCell(playerPos.x, playerPos.y);
        
        // 更新迷雾视野
        if (fogModeEnabled) {
            fogOfWar->updateVisibility(playerPos);
        }
        
        // 检查陷阱
        if (currentCell == TRAP) {
            player.takeDamage(30);
            // 陷阱消失
            const_cast<Map*>(currentMap)->setCell(playerPos.x, playerPos.y, EMPTY);
        }
        
        currentPathIndex++;
    } else {
        // 移动失败，重新计算路径
        stopAutoMode();
        calculatePath();
        if (!currentPath.empty()) {
            startAutoMode();
        }
    }
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
    
    if (autoModeEnabled) {
        cout << "自动模式: " << (autoModeRunning ? "运行中" : "就绪") << "\n";
        if (!currentPath.empty() && autoModeRunning) {
            cout << "路径进度: " << currentPathIndex << "/" << currentPath.size() << "\n";
        }
    }
    cout << "\n";
    
    // 显示地图
    if (fogModeEnabled && fogOfWar) {
        displayMapWithFog();
    } else {
        for (int y = 0; y < currentMap->getHeight(); y++) {
            for (int x = 0; x < currentMap->getWidth(); x++) {
                Position playerPos = player.getPosition();
                
                // 显示路径
                bool isPath = false;
                if (autoModeRunning && !currentPath.empty()) {
                    for (size_t i = currentPathIndex; i < currentPath.size(); i++) {
                        if (currentPath[i].x == x && currentPath[i].y == y) {
                            cout << ". ";
                            isPath = true;
                            break;
                        }
                    }
                }
                
                if (isPath) continue;
                
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
    }
    
    // 图例
    cout << "图例: P=玩家, #=墙壁, x=陷阱, S=起点, E=终点";
    if (autoModeRunning && !currentPath.empty()) {
        cout << ", .=规划路径";
    }
    if (fogModeEnabled) {
        cout << ", ?=未探索区域";
    }
    cout << "\n";
    
    if (autoModeEnabled && !currentPath.empty() && !autoModeRunning) {
        cout << "找到路径! 长度: " << currentPath.size() << " 步\n";
    }
    
    cout << "----------------------------------------\n";
}

// 其他现有函数保持不变...
void Game::displayMapWithFog() const {
    // 保持原有实现，这里省略以节省空间
    Position playerPos = player.getPosition();
    
    for (int y = 0; y < currentMap->getHeight(); y++) {
        for (int x = 0; x < currentMap->getWidth(); x++) {
            FogState fogState = fogOfWar->getFogState(x, y);
            
            if (fogState == FOG_UNEXPLORED) {
                cout << "? ";
                continue;
            }
            
            // 显示路径（在自动模式下）
            bool isPath = false;
            if (autoModeRunning && !currentPath.empty()) {
                for (size_t i = currentPathIndex; i < currentPath.size(); i++) {
                    if (currentPath[i].x == x && currentPath[i].y == y) {
                        cout << ". ";
                        isPath = true;
                        break;
                    }
                }
            }
            
            if (isPath) continue;
            
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
    
    if (autoModeEnabled && !currentPath.empty()) {
        cout << "最优路径长度: " << currentPath.size() << " 步\n";
        cout << "实际步数/最优步数: " << player.getSteps() << "/" << currentPath.size() << "\n";
    }
}