// DevCardLoop.h (已集成 UI 循环、随机效果并移除所有输出)

#include <graphics.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <cstdlib> // 引入 rand()
#include <ctime>   // 引入 time()
#include "Player.h"
#include "DevCardManager.h"
#include "DevCardPanel.h"
#include "windows.h"
#include "TextWindow.h" // 假设存在，但我们不会调用任何输出函数

// 假设 ActionType, DevCardType, ResourceType 在 Common.h 或其他头文件定义
// enum class ActionType { None, Knight, RoadBuilding, ... };
// 假设 ResourceType 枚举值为 0 到 4 (WOOD, BRICK, SHEEP, WHEAT, ORE)

ActionType DevCardLoop(Player& player1) {

    // --- 0. 随机数初始化 (确保随机性) ---
    // 建议在游戏启动时调用一次，这里为保证效果在函数内调用
    srand((unsigned int)time(NULL));

    // --- 1. 初始化部分 ---
    cleardevice();
    BeginBatchDraw();

    // 注意：这里的 allPlayers 列表应包含游戏中的所有玩家
    std::vector<Player*> allPlayers = { &player1 };
    // 【重要】：如果您的 DevCardManager 构造函数需要完整的玩家列表，请确保 allPlayers 包含所有玩家指针
    DevCardManager manager(allPlayers);

    // 创建 DevCardPanel 对象
    DevCardPanel panel(130, GetSystemMetrics(SM_CYSCREEN) - 165);

    // 加载背景图片
    panel.loadBackgroundImage("assets/dev_bg.jpg", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) - 30);

    // --- 2. 界面循环部分 ---
    panel.setVisible(true); // 使面板可见

    int chosenCard = -1; // 用于存储用户最终确认的卡牌类型

    // 核心 UI 交互循环
    while (chosenCard == -1) {

        panel.update(player1, manager); // 更新卡牌数量和启用状态

        // --- 获取当前鼠标位置 (用于绘制悬停效果) ---
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(GetHWnd(), &mousePos);
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        cleardevice();

        // --- 绘制 ---
        panel.draw(manager, player1, mouseX, mouseY);

        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
            if (msg.message == WM_LBUTTONDOWN) {
                int result = panel.handleClick(msg.x, msg.y);

                if (result >= 0) {
                    chosenCard = result; // 成功点击确认，chosenCard 被赋值为卡牌类型
                    break;
                }

                if (result == -2) {
                    chosenCard = -2; // 点击返回按钮
                    break;
                }
            }
        }

        // 如果按 ESC，返回 -2 (取消)
        if (_kbhit() && _getch() == 27) {
            chosenCard = -2; // 确保 ESC 也被视为取消操作
            break;
        }

        Sleep(10);
        FlushBatchDraw(); // 刷新画面
    }

    // --- 3. 结果处理和 Manager 调用 (核心逻辑：实现随机效果并移除输出) ---

    Sleep(500);
    EndBatchDraw();

    ActionType nextAction = ActionType::None;

    if (chosenCard >= 0) { // 用户确认使用了一张卡牌
        DevCardType cardType = (DevCardType)chosenCard;

        // 【核心集成点】：调用 Manager::PlayCard 来扣除卡牌并执行规则检查
        if (manager.PlayCard(player1.getID(), cardType)) {

            // 定义所有资源类型以便随机选择 (假设 WOOD=0, BRICK=1, ..., ORE=4, 共 5 种)
            // 请根据您的 Common.h 确保 ResourceType 的枚举定义
            const int NUM_RESOURCES = 5;
            ResourceType allResources[] = { WOOD, BRICK, SHEEP, WHEAT, ORE };

            // 成功使用卡牌，设置返回的 ActionType 或执行即时效果
            switch (cardType) {
            case KNIGHT:
                nextAction = ActionType::Knight;
                break;
            case ROAD_BUILDING:
                nextAction = ActionType::RoadBuilding;
                break;
            case YEAR_OF_PLENTY: {
                // 丰饶年：随机获得 2 种资源 (直接给玩家添加资源)
                ResourceType res1 = allResources[rand() % NUM_RESOURCES];
                ResourceType res2 = allResources[rand() % NUM_RESOURCES];
                ResourceType res3 = allResources[rand() % NUM_RESOURCES];
                ResourceType res4 = allResources[rand() % NUM_RESOURCES];
                ResourceType res5 = allResources[rand() % NUM_RESOURCES];
                ResourceType res6 = allResources[rand() % NUM_RESOURCES];

                player1.addResource(res5, 1);
                player1.addResource(res6, 1);
                player1.addResource(res1, 1);
                player1.addResource(res2, 1);
                player1.addResource(res3, 1);
                player1.addResource(res4, 1);
                break;
            }
            case MONOPOLY: {
                // 垄断：随机选择一种资源
                ResourceType chosenType = allResources[rand() % NUM_RESOURCES];
                player1.addResource(chosenType, 10);
                //manager.Monopoly(player1.getID(), chosenType, allPlayers);
                // nextAction 保持 None
                break;
            }
            case VICTORY_POINT:
                break;
            default:
                break;
            }
        }
        else {
            nextAction = ActionType::None;
        }
    }

    // 如果 chosenCard == -2 (返回) 或 < 0 (错误)，nextAction 保持 None
    return nextAction;
}