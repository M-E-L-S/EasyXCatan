#include "DevCardShow.h" // 假设包含函数声明
#include <graphics.h>
#include <conio.h>
#include <windows.h>
// 假设头文件 DevCardPanel.h, Player.h, DevCardManager.h 已经被包含进来

int ShowDevCardPanel(Player& player, DevCardManager& manager, DevCardPanel& panel)
{
    panel.setVisible(true);
    panel.update(player, manager);

    // 【重要】：为了防止画面闪烁并实现流畅的悬停，这里应该使用 BeginBatchDraw/EndBatchDraw
    // 假设在 main() 或更高层级已经调用了 BeginBatchDraw()

    while (true) {
        // --- 1. 获取当前鼠标位置 ---
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(GetHWnd(), &mousePos);
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        cleardevice();

        // --- 2. 传入鼠标坐标进行绘制 ---
        panel.draw(manager, player, mouseX, mouseY);

        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE | EM_KEY)) { // 确保能捕获鼠标和键盘事件
            if (msg.message == WM_LBUTTONDOWN) {
                int result = panel.handleClick(msg.x, msg.y);

                if (result >= 0 && result < DEV_CARD_COUNT) {
                    return result; // 返回卡牌类型
                }

                if (result == -2) {
                    return -1; // 返回按钮 (取消/关闭)
                }
            }
        }

        // 如果按 ESC，返回 -1
        if (_kbhit() && _getch() == 27)
            return -1;

        Sleep(10);
        FlushBatchDraw(); // 刷新画面
    }
}