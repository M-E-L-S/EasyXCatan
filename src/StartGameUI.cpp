#include "StartGameUI.h"
#include <iostream> 
#include <conio.h>  // 用于 _kbhit() 和 _getch()
#include <windows.h> // 用于 Windows API 调用和 GetHWND
#include <tchar.h>   // 用于 _T() 宏

// 构造函数
StartGameUI::StartGameUI(int width, int height)
    : windowWidth(width),
    windowHeight(height),
    // 初始化“开始游戏”按钮 (位置居中)
    startButton(windowWidth / 2 - 100, windowHeight / 2 + 50, 200, 60, "开始游戏", RGB(38, 111, 64)),
    // 初始化“退出游戏”按钮 (位于开始游戏按钮下方)
    exitButton(windowWidth / 2 - 100, windowHeight / 2 + 130, 200, 60, "退出游戏", RGB(200, 60, 60))
{
    // 确保按钮是可点击的
    startButton.enabled = true;
    exitButton.enabled = true;

    // 加载背景图片 (请确保 assets/start_bg.png 路径正确)
    loadBackgroundImage("assets/start_bg.jpg");
}

// 【关键实现】：加载图片并自适应屏幕
void StartGameUI::loadBackgroundImage(const char* filePath) {
    // 使用 _T() 宏和 5 参数 loadimage，强制将图片缩放到窗口尺寸
    loadimage(&backgroundImage, _T(filePath), windowWidth, windowHeight, true);
}

// 绘制整个界面
void StartGameUI::draw(int mouseX, int mouseY) {
    // 绘制背景图片
    putimage(0, 0, &backgroundImage);

    // 绘制“开始游戏”按钮及其悬停效果
    startButton.draw();

    int btnX = startButton.x;
    int btnY = startButton.y;
    int btnW = startButton.w;
    int btnH = startButton.h;

    bool isHoverStart = (mouseX >= btnX && mouseX <= btnX + btnW &&
        mouseY >= btnY && mouseY <= btnY + btnH);

    if (isHoverStart && startButton.enabled) {
        setlinecolor(RGB(255, 255, 150)); // 黄色边框
        setlinestyle(PS_SOLID, 3);
        rectangle(btnX, btnY, btnX + btnW, btnY + btnH);
    }

    // 绘制“退出游戏”按钮及其悬停效果
    exitButton.draw();

    btnX = exitButton.x;
    btnY = exitButton.y;
    btnW = exitButton.w;
    btnH = exitButton.h;

    bool isHoverExit = (mouseX >= btnX && mouseX <= btnX + btnW &&
        mouseY >= btnY && mouseY <= btnY + btnH);

    if (isHoverExit && exitButton.enabled) {
        setlinecolor(RGB(255, 255, 150)); // 黄色边框
        setlinestyle(PS_SOLID, 3);
        rectangle(btnX, btnY, btnX + btnW, btnY + btnH);
    }
    setlinestyle(PS_SOLID, 1); // 恢复默认线型
}

// 显示界面并处理用户交互
bool StartGameUI::show() {
    cleardevice();     // 清空屏幕
    BeginBatchDraw();  // 开始批量绘制

    bool gameStarted = false;

    while (!gameStarted) {
        // --- 获取当前鼠标位置 ---
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(GetHWnd(), &mousePos);
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        // --- 绘制界面 ---
        draw(mouseX, mouseY);

        // --- 处理消息 ---
        ExMessage msg;
        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {

                // 1. 如果“开始游戏”按钮被点击
                if (startButton.isClicked(msg.x, msg.y)) {
                    gameStarted = true; // 设置标志，返回 true
                    goto end_loop;
                }

                // 2. 如果“退出游戏”按钮被点击
                if (exitButton.isClicked(msg.x, msg.y)) {
                    gameStarted = false; // 设置标志，返回 false
                    goto end_loop;
                }
            }
        }

        // 使用 _kbhit() / _getch() 来检查 ESC 键（ASCII 27）
        if (_kbhit()) {
            if (_getch() == 27) {
                gameStarted = false; // 用户选择退出
                goto end_loop;
            }
        }

        Sleep(10);        // 短暂暂停，避免CPU占用过高
        FlushBatchDraw(); // 刷新画面
    }

end_loop:; // goto 标签

    EndBatchDraw(); // 结束批量绘制
    return gameStarted; // 返回是否开始游戏 (true/false)
}