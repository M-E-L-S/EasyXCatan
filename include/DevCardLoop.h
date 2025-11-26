#include <graphics.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include "Player.h"
#include "DevCardManager.h"
#include "DevCardShow.h"
#include "DevCardPanel.h"
#include "windows.h"


ActionType DevCardLoop(Player& player1) {

    cleardevice();
    BeginBatchDraw();

    std::vector<Player*> allPlayers = { &player1};
    DevCardManager manager(allPlayers);

    // 创建 DevCardPanel 对象:参数改变按钮的位置
    DevCardPanel panel(130, GetSystemMetrics(SM_CYSCREEN) - 165);

    // 加载背景图片
    panel.loadBackgroundImage("assets/dev_bg.jpg", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-30);

    // 调用接口显示面板
    int chosenCard = ShowDevCardPanel(player1, manager, panel);

    Sleep(500);
    EndBatchDraw();

    //返回需要与地图交互的动作类型
    if(chosenCard== (int)KNIGHT)
        return ActionType::Knight;
    else if(chosenCard==(int)ROAD_BUILDING)
        return ActionType::RoadBuilding;
    else
        return ActionType::None;
}