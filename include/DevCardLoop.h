#include <graphics.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include "Player.h"
#include "DevCardManager.h"
#include "DevCardShow.h"
#include "DevCardPanel.h"
#include "windows.h"


void DevCardLoop(Player& player1) {

    cleardevice();
    BeginBatchDraw();

    std::vector<Player*> allPlayers = { &player1};
    DevCardManager manager(allPlayers);

    // 在 main 中创建 DevCardPanel 对象:改变按钮的位置
    DevCardPanel panel(130, GetSystemMetrics(SM_CYSCREEN) - 165);

    // [新增] 加载背景图片
    // 假设你的窗口是 1280x720，你可以传入宽高达强制拉伸铺满
    // 或者直接 panel.loadBackgroundImage("assets/dev_bg.jpg");
    panel.loadBackgroundImage("assets/dev_bg.jpg", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-30);

    // 调用接口显示面板
    int chosenCard = ShowDevCardPanel(player1, manager, panel);

    if (chosenCard >= 0) {
        std::cout << "玩家选择了发展卡类型: " << chosenCard << std::endl;
    }
    else {
        std::cout << "玩家取消或关闭了面板" << std::endl;
    }

    Sleep(500);
    EndBatchDraw();
    //closegraph();
}