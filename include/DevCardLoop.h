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
    //initgraph(1920, 1080);
    //setbkcolor(WHITE);
    cleardevice();
    BeginBatchDraw();

    std::vector<Player*> allPlayers = { &player1};
    DevCardManager manager(allPlayers);

    // 在 main 中创建 DevCardPanel 对象
    DevCardPanel panel(10, 30);

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