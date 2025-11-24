#pragma once
#include <graphics.h>
#include "BankPanel.h"
#include "Player.h"

// 银行模块的入口函数
// 传入当前玩家引用，以便修改资源
inline void EnterBankMode(Player& currentPlayer) {
    // 1. 获取当前窗口大小 (适配全屏)
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    // 2. 创建银行实例
    BankPanel bank(w, h);

    // 3. 加载资源 (确保 assets 文件夹在 exe 同级目录)
    const char* iconPaths[RESOURCE_COUNT] = {
        "./assets/wood.jpg",
        "./assets/brick.jpg",
        "./assets/sheep.jpg",
        "./assets/wheat.jpg",
        "./assets/ore.jpg"
    };
    bank.loadResourceImages(iconPaths);

    // 加载背景图 (如果没有会自动显示羊皮纸色块)
    // 请确保 assets/bank_bg.png 存在，否则会显示纯色背景
    bank.loadBackgroundImage("./assets/bank_bg.png");

    // ==========================================
    // 【关键修复 1】设置兜底背景色为白色
    // ==========================================
    setbkcolor(WHITE);
    cleardevice();

    // ==========================================
    // 【关键修复 2】显式开启批量绘图
    // 即使主循环里开过，这里是独立子循环，必须确保双缓冲开启
    // 防止闪屏，保证按钮和背景同时出现
    // ==========================================
    BeginBatchDraw();

    bool isBankOpen = true;
    ExMessage msg;

    // 用于记录鼠标位置以实现高亮
    int mouseX = 0;
    int mouseY = 0;

    while (isBankOpen) {
        // --- A. 处理输入 ---
        // 使用 while 循环处理所有积压消息，保证交互流畅
        while (peekmessage(&msg, EM_MOUSE | EM_KEY, PM_REMOVE)) {

            // 更新鼠标位置用于 draw
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }

            // 处理 ESC 键退出
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                isBankOpen = false;
            }

            // 将消息传递给银行面板逻辑
            // handleInput 返回 true 代表用户点击了关闭(X)按钮
            if (bank.handleInput(msg, currentPlayer)) {
                isBankOpen = false;
            }
        }

        // --- B. 绘图 ---
        // 1. 先擦除上一帧 (使用我们设置的黑色背景)
        cleardevice();

        // 2. 绘制所有银行内容 (背景图、按钮、图标)
        // 传入 mouseX, mouseY 以实现悬停高亮效果
        bank.draw(currentPlayer, mouseX, mouseY);

        // 3. 统一提交这一帧的绘图
        FlushBatchDraw();

        // 4. 帧率控制 (约 60 FPS)
        Sleep(16);
    }

    // ==========================================
    // 【关键修复 3】退出时结束本次批量绘图
    // ==========================================
    EndBatchDraw();
}