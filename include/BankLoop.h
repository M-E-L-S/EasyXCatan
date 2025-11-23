#pragma once
#include <graphics.h>
#include "BankPanel.h"
#include "Player.h"

// 为了方便，我们将银行逻辑封装为一个内联函数
// 在实际大项目中，通常会放入 .cpp 文件，但在测试结构中 inline 很方便
inline void EnterBankMode(Player& currentPlayer) {
    // 1. 获取当前窗口大小
    int w = getwidth();
    int h = getheight();

    // 2. 创建银行实例
    BankPanel bank(w, h);

    // 3. 加载资源 (硬编码路径，确保 assets 文件夹存在)
    const char* iconPaths[RESOURCE_COUNT] = {
        "assets/wood.png",
        "assets/brick.png",
        "assets/sheep.png",
        "assets/wheat.png",
        "assets/ore.png"
    };
    bank.loadResourceImages(iconPaths);
    bank.loadBackgroundImage("assets/bank_bg.png");

    // 4. 进入银行专属的消息循环 (Modal Loop)
    // 这个循环会阻塞主程序的运行，直到玩家关闭银行
    bool isBankOpen = true;
    ExMessage msg;

    // 用于记录鼠标位置以实现高亮
    int mouseX = 0;
    int mouseY = 0;

    // 刷新一次以防止闪屏
    FlushBatchDraw();

    while (isBankOpen) {
        // --- A. 处理输入 ---
        // 使用 while 循环处理所有积压消息，保证流畅
        while (peekmessage(&msg, EM_MOUSE | EM_KEY, PM_REMOVE)) {

            // 1. 更新鼠标位置
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }

            // 2. 处理 ESC 键退出
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                isBankOpen = false;
            }

            // 3. 将消息传递给银行逻辑
            // 如果 handleInput 返回 true，说明点击了关闭按钮
            if (bank.handleInput(msg, currentPlayer)) {
                isBankOpen = false;
            }
        }

        // --- B. 绘图 ---
        cleardevice();

        // 调用银行的绘制函数
        bank.draw(currentPlayer, mouseX, mouseY);

        FlushBatchDraw();

        // 稍微休眠，避免空转占用 100% CPU
        Sleep(10);
    }

    // 循环结束，函数返回，控制权交回给 TestBank.cpp
}