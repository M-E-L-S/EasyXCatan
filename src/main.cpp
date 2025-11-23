#include <graphics.h>
#include <tchar.h>
#include <cstdio>
#include "BankPanel.h"
#include "Player.h"
#include "Button.h"

// --- 全局变量 ---
Button* btnOpenBank = nullptr;
bool isBankPanelOpen = false;
// 用于记录当前鼠标位置，实现悬停(Hover)高亮效果
int currentMouseX = 0;
int currentMouseY = 0;

// 辅助函数：在屏幕下方打印调试信息
void DrawDebugInfo(const Player& p) {
    settextcolor(YELLOW);
    settextstyle(20, 0, _T("Consolas"));
    setbkmode(TRANSPARENT);

    TCHAR buffer[256];
    _stprintf_s(buffer, _T("RESOURCES: Wood:%d | Brick:%d | Sheep:%d | Wheat:%d | Ore:%d"),
        p.getResourceCount(WOOD), p.getResourceCount(BRICK),
        p.getResourceCount(SHEEP), p.getResourceCount(WHEAT), p.getResourceCount(ORE));

    // 画在屏幕最底部
    outtextxy(10, 730, buffer);
}

// 模拟主界面背景
void DrawMainScreenPlaceholder() {
    settextcolor(LIGHTGRAY);
    settextstyle(40, 0, _T("Arial Black"));
    // 居中显示文字
    outtextxy(200, 300, _T("CATAN MAIN GAME MAP (Placeholder)"));

    settextstyle(20, 0, _T("Arial"));
    outtextxy(350, 360, _T("Press 'Open Bank' to trade resources."));
}

int main() {
    // 1. 初始化 1024x768 窗口
    initgraph(1024, 768);
    setbkcolor(RGB(0, 50, 100)); // 深蓝背景模拟大海

    // 2. 初始化测试玩家
    // 给玩家足够的资源来测试 "置灰" 和 "高亮"
    // 场景：有很多砖和羊（可支付），但木头很少（不可支付）
    Player testPlayer(0, RED);
    testPlayer.addResource(BRICK, 20);
    testPlayer.addResource(SHEEP, 20);
    testPlayer.addResource(WOOD, 2);   // < 4，应该显示为置灰状态
    testPlayer.addResource(WHEAT, 0);
    testPlayer.addResource(ORE, 0);

    // 3. 创建主界面上的打开按钮
    btnOpenBank = new Button(50, 650, 150, 50, "Open Bank", RGB(100, 200, 100));

    // 4. 创建银行面板
    BankPanel bank(1024, 768);
    // 加载图片 (请确保 assets 文件夹下有这些图片，如果没有，代码会自动画框代替)
    const char* paths[RESOURCE_COUNT] = {
        "assets/wood.png", "assets/brick.png", "assets/sheep.png", "assets/wheat.png", "assets/ore.png"
    };
    bank.loadResourceImages(paths);

    BeginBatchDraw();

    bool isRunning = true;
    while (isRunning) {
        // --- 1. 处理输入消息 ---
        ExMessage msg;
        // 使用 peekmessage 循环处理所有积压的消息，防止输入延迟
        while (peekmessage(&msg, EM_MOUSE | EM_KEY, PM_REMOVE)) {

            // 实时更新鼠标位置变量 (用于 draw 函数中的悬停高亮)
            if (msg.message == WM_MOUSEMOVE) {
                currentMouseX = msg.x;
                currentMouseY = msg.y;
            }

            // 处理 ESC 键
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                if (isBankPanelOpen) isBankPanelOpen = false; // 关闭银行
                else isRunning = false; // 退出程序
            }

            // 根据当前状态分发消息
            if (isBankPanelOpen) {
                // 【关键修改】直接把 msg 传给银行，因为它要判断 左键加/右键减
                // handleInput 返回 true 代表用户点击了关闭(X)按钮
                if (bank.handleInput(msg, testPlayer)) {
                    isBankPanelOpen = false;
                }
            }
            else {
                // 主界面交互：点击打开按钮
                if (msg.message == WM_LBUTTONDOWN) {
                    if (btnOpenBank->isClicked(msg.x, msg.y)) {
                        isBankPanelOpen = true;
                    }
                }
            }
        }

        // --- 2. 绘图逻辑 ---
        cleardevice();

        // 始终显示底部的调试信息
        DrawDebugInfo(testPlayer);

        if (isBankPanelOpen) {
            // 【关键修改】传入 currentMouseX/Y 以实现“悬停高亮”效果
            bank.draw(testPlayer, currentMouseX, currentMouseY);
        }
        else {
            // 绘制主界面
            DrawMainScreenPlaceholder();
            btnOpenBank->draw();
        }

        FlushBatchDraw();
        Sleep(10); // 稍微休眠，降低CPU占用
    }

    // 清理内存
    delete btnOpenBank;
    closegraph();
    return 0;
}