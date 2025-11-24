// Button.h
#pragma once
#include <graphics.h>
#include <string>

// 一个简单的可复用按钮结构体
struct Button {
    int x, y, w, h;     // 位置和宽高
    std::string text;   // 按钮上的文字
    COLORREF color;     // 按钮颜色
    bool enabled;       // 按钮是否可点击 (用于 "灰显")

    Button()
        :x(0), y(0), w(0), h(0), text(""), color(RGB(0, 0, 0)), enabled(false) {}

    Button(int x, int y, int w, int h, const std::string& text, COLORREF color) :
        x(x), y(y), w(w), h(h), text(text), color(color), enabled(true) {}

    // 绘制按钮
    void draw() const {

        // 定义圆角半径
        const int CORNER_RADIUS = 30;

        // 设置填充色 (根据是否启用)
        setfillcolor(enabled ? color : RGB(128, 128, 128));
        // 使用 fillroundrect 绘制填充的圆角矩形
        fillroundrect(x, y, x + w, y + h, CORNER_RADIUS, CORNER_RADIUS);

        // 绘制边框
        setlinecolor(RGB(128, 128, 128));
        // 使用 roundrect 绘制圆角边框
        roundrect(x, y, x + w, y + h, CORNER_RADIUS, CORNER_RADIUS);

        // 绘制文字 (居中)
        settextcolor(enabled ? BLACK : DARKGRAY);
        setbkmode(TRANSPARENT);
        // 1. 获取 C 风格字符串
        const char* c_str = text.c_str();

        // 2. 使用 c_str 进行宽度和高度计算
        // 假设您的 EasyX 采用的是多字节字符集 (MBCS)
        int textW = textwidth(c_str);
        int textH = textheight(c_str);

        // 3. 绘制文字
        outtextxy(x + (w - textW) / 2, y + (h - textH) / 2, c_str);

    }

    // 检查是否被点击 (必须是启用的)
    bool isClicked(int mouseX, int mouseY) const {
        return enabled && (mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h);
    }
};