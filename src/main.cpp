#include <graphics.h>
#include <tchar.h>
#include <cstdio>
#include "Player.h"
#include "Button.h"

// 引入我们刚才写的银行循环模块
#include "BankLoop.h"

// 辅助：显示调试信息
void DrawDebugInfo(const Player& p) {
    settextcolor(YELLOW);
    settextstyle(20, 0, _T("Consolas"));
    setbkmode(TRANSPARENT);
    TCHAR buffer[256];
    _stprintf_s(buffer, _T("Main Game State - Resources: Wood:%d | Brick:%d | Sheep:%d | Wheat:%d | Ore:%d"),
        p.getResourceCount(WOOD), p.getResourceCount(BRICK),
        p.getResourceCount(SHEEP), p.getResourceCount(WHEAT), p.getResourceCount(ORE));
    outtextxy(10, 730, buffer);
}

// 模拟主游戏地图背景
void DrawGameMapPlaceholder() {
    setbkcolor(RGB(0, 50, 100));
    cleardevice();

    settextcolor(LIGHTGRAY);
    settextstyle(50, 0, _T("Arial Black"));
    outtextxy(250, 300, _T("CATAN GAME MAP"));

    settextstyle(25, 0, _T("Arial"));
    outtextxy(320, 380, _T("Everything here is the main game loop."));
}

int main() {
    // 1. 初始化窗口
    initgraph(1024, 768);

    // 2. 初始化玩家数据
    Player myPlayer(0, RED);
    myPlayer.addResource(BRICK, 20);
    myPlayer.addResource(SHEEP, 20);
    myPlayer.addResource(WOOD, 2);

    // 3. 主界面按钮
    Button btnOpenBank(412, 500, 200, 60, "Open Bank", RGB(0, 120, 215));

    BeginBatchDraw();

    bool isGameRunning = true;
    while (isGameRunning) {
        // --- 主游戏输入处理 ---
        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE | EM_KEY, PM_REMOVE)) {
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                isGameRunning = false; // 退出游戏
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                // 如果点击了“打开银行”按钮
                if (btnOpenBank.isClicked(msg.x, msg.y)) {

                    // ==============================================
                    // 关键点：只需一行代码，即可进入银行系统！
                    // 主游戏会在这里“暂停”，直到银行界面关闭
                    // ==============================================
                    EnterBankMode(myPlayer);

                    // 从银行回来后，重置背景色防止闪烁
                    setbkcolor(RGB(0, 50, 100));
                }
            }
        }

        // --- 主游戏绘图 ---
        DrawGameMapPlaceholder();
        DrawDebugInfo(myPlayer);
        btnOpenBank.draw();

        FlushBatchDraw();
        Sleep(10);
    }

    closegraph();
    return 0;
}