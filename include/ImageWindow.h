#pragma once

#include <graphics.h>
#include <string>
#include "Button.h"
#include "Common.h"

// 定义模态框的颜色和尺寸
#define MODAL_WIDTH 400
#define MODAL_HEIGHT 250        // 稍微加高一点，给图片留出更多空间
#define MODAL_BG_COLOR RGB(195,171,140)
#define MODAL_BORDER_COLOR RGB(50, 50, 50)
#define BUTTON_COLOR RGB(194,37,37)

// 修改函数名为 showImageWindow，参数改为 imagePath
void showImageWindow(const std::string& imagePath) {
    // 自动获取屏幕/窗口中心坐标
    int screenW = SCREEN_WIDTH;
    int screenH = SCREEN_HEIGHT;

    int modalX = (screenW - MODAL_WIDTH) / 2;
    int modalY = (screenH - MODAL_HEIGHT) / 2;

    // --- 图片处理核心逻辑 ---
    IMAGE img;

    // 定义图片在模态框中的最大显示尺寸（留出边缘和按钮的空间）
    // 宽度减去左右边距(40)，高度减去底部按钮和顶部边距(80)
    int maxImgW = MODAL_WIDTH - 40;
    int maxImgH = MODAL_HEIGHT - 80;

    // 加载图片并自动缩放至指定大小 (防止图片过大撑爆窗口)
    // 注意：这里强制拉伸到指定宽高，如果需要保持比例，需要写更复杂的算法
    loadimage(&img, (LPCTSTR)imagePath.c_str(), maxImgW, maxImgH);

    // 计算图片居中显示的坐标
    int imgX = modalX + (MODAL_WIDTH - maxImgW) / 2;
    int imgY = modalY + 20; // 距离顶部 20 像素

    // -----------------------

    // 创建一个确认按钮
    int btnW = 100;
    int btnH = 40;
    // 按钮位置: 模态框右下角
    int btnX = modalX + MODAL_WIDTH - btnW - 20;
    int btnY = modalY + MODAL_HEIGHT - btnH - 15;

    Button confirmButton(btnX, btnY, btnW, btnH, "Return", BUTTON_COLOR);

    BeginBatchDraw();

    bool exitModal = false;

    while (!exitModal) {
        // --- 绘制模态框主体 ---
        // 绘制阴影
        setfillcolor(RGB(140, 120, 100)); // 稍微深一点的颜色做阴影
        solidroundrect(modalX + 5, modalY + 5, modalX + MODAL_WIDTH + 5, modalY + MODAL_HEIGHT + 5, 8, 8);

        // 绘制背景
        setfillcolor(MODAL_BG_COLOR);
        setlinecolor(MODAL_BORDER_COLOR);
        fillroundrect(modalX, modalY, modalX + MODAL_WIDTH, modalY + MODAL_HEIGHT, 8, 8);

        // --- 绘制图片 (替代原来的文字) ---
        // 把加载好的 img 对象画出来
        putimage(imgX, imgY, &img);

        // --- 绘制按钮 ---
        confirmButton.draw();

        FlushBatchDraw();

        // --- 输入检测 ---
        ExMessage msg;
        msg = getmessage(EM_MOUSE | EM_KEY);

        if (msg.message == WM_LBUTTONDOWN) {
            if (confirmButton.isClicked(msg.x, msg.y)) {
                exitModal = true;
            }
        }
    }

    EndBatchDraw();

    // 释放模态框占用的资源（此处 IMAGE 对象 img 是局部变量，函数结束会自动销毁，无需手动释放）
}