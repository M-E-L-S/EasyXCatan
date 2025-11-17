// Button.h
#pragma once
#define GRAY RGB(128, 128, 128)
#include <graphics.h>
#include <string>

struct Button {
    int x, y, w, h;
    std::string text;
    COLORREF color;
    bool enabled;

    Button(int x, int y, int w, int h, const std::string& text, COLORREF color) :
        x(x), y(y), w(w), h(h), text(text), color(color), enabled(true) {}

    // 绘制按钮
    void draw() const {
        // 根据是否启用，选择不同的填充色
        if (enabled) {
            setfillcolor(color);
        } else {
            setfillcolor(GRAY); // 灰显
        }
        solidrectangle(x, y, x + w, y + h);

        // 绘制边框
        setlinecolor(GRAY);
        rectangle(x, y, x + w, y + h);

        // 绘制文字 (居中)
        if (enabled) {
            settextcolor(BLACK);
        } else {
            settextcolor(DARKGRAY); // 灰显文字
        }
        setbkmode(TRANSPARENT);

        // EasyX 的 C-style 字符串处理
        const char* btnText = text.c_str();
        int textW = textwidth(btnText);
        int textH = textheight(btnText);
        outtextxy(x + (w - textW) / 2, y + (h - textH) / 2, btnText);
    }

    // 检查是否被点击 (必须是启用的)
    bool isClicked(int mouseX, int mouseY) const {
        return enabled &&
               (mouseX >= x && mouseX <= x + w &&
                mouseY >= y && mouseY <= y + h);
    }
};