#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <string>
#include "Common.h"
#include "Player.h"
#include "PlayerPanel.h"
#include "PlayerLoop.h" 
#include <windows.h>
#include <vector>

std::vector<Player*> allPlayers;
DevCardManager* devCardManager = nullptr;

int main() {
    // 初始化图形窗口
    initgraph(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    // 设置背景色为白色，防止某些未覆盖区域是黑色的
    setbkcolor(WHITE);
    cleardevice();

    // 2. 初始化测试数据
    // 创建 ID为0，颜色为红色的玩家
    Player p1(0, RED);



    // --- 给玩家添加一些初始资源，方便你调试 UI ---
    // 比如：给足够的资源造路和村庄
    p1.addResource(WOOD, 5);
    p1.addResource(BRICK, 5);
    p1.addResource(SHEEP, 2);
    p1.addResource(WHEAT, 2);
    p1.addResource(ORE, 1);

    // 添加一个村庄 (为了测试 "Build City" 按钮，因为建城市需要先有村庄)
    p1.addBuilding(SETTLEMENT);

    // 初始游戏状态
    GameState currentState = GameState::IDLE;
    int lastDiceRoll = 8; // 假设上次掷出了8点

    // 3. 提示信息
    /*settextcolor(BLACK);
    settextstyle(20, 0, _T("Consolas"));
    outtextxy(10, 10, _T("按任意键进入 PlayerLoop..."));
    _getch();*/

    // 4. 进入你的 PlayerLoop
    // 注意：PlayerLoop.h 中定义的函数是 void PlayerLoop(...)
    // 循环结束后，通常是因为玩家点击了 End Turn 或 Switch to Map
    PlayerLoop(p1, currentState, lastDiceRoll);

    // 5. 循环结束后的反馈
    cleardevice();
    settextcolor(BLACK);
    settextstyle(30, 0, _T("Arial"));

    std::string msg = "Exited PlayerLoop. New State: " + std::to_string((int)currentState);
    outtextxy(100, 300, msg.c_str());
    outtextxy(100, 350, "Press any key to close...");

    _getch();
    closegraph();
    return 0;
}