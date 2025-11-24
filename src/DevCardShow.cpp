#include "DevCardShow.h"
#include <graphics.h>
#include <conio.h>
#include <windows.h>

int ShowDevCardPanel(Player& player, DevCardManager& manager, DevCardPanel& panel)
{
    panel.setVisible(true);
    panel.update(player, manager);

    while (true) {
        cleardevice();
        panel.draw(manager, player);

        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                int result = panel.handleClick(msg.x, msg.y);

                // 点击确认后返回卡牌类型
                if (result >= 0 && result < DEV_CARD_COUNT) {
                    return result;
                }

                // 处理返回按钮点击 (-2)
                if (result == -2) {
                    return -1; // 统一返回 -1 表示取消/关闭
                }
            }
        }

        // 如果按 ESC，返回 -1
        if (_kbhit() && _getch() == 27)
            return -1;

        Sleep(10);
        FlushBatchDraw();
    }
}