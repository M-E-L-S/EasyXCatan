#pragma once

#include <graphics.h>
#include "Button.h"   
#include <string>     

// StartGameUI 类用于显示开始游戏界面
class StartGameUI {
public:
    StartGameUI(int windowWidth, int windowHeight); // 构造函数，传入窗口尺寸

    // 显示界面并处理用户交互，直到“开始游戏”按钮或“退出游戏”按钮被点击
    // 返回 true 表示点击了开始游戏，返回 false 表示退出或错误
    bool show();

private:
    int windowWidth;    // 窗口宽度
    int windowHeight;   // 窗口高度
    IMAGE backgroundImage; // 背景图片对象
    Button startButton;    // “开始游戏”按钮
    Button exitButton;     // “退出游戏”按钮

    // 绘制整个界面
    void draw(int mouseX, int mouseY);

    // 【关键】加载背景图片并自适应屏幕
    void loadBackgroundImage(const char* filePath);
};