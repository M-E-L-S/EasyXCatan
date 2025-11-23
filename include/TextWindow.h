#pragma once

#include <graphics.h>
#include <string>
#include "Button.h" // 依赖你提供的 Button.h 文件
#include "Common.h"

// 定义模态框的颜色和尺寸
#define MODAL_WIDTH 400
#define MODAL_HEIGHT 200
#define MODAL_BG_COLOR RGB(245, 245, 245) // 浅灰色背景
#define MODAL_BORDER_COLOR RGB(50, 50, 50)  // 深色边框
#define BUTTON_COLOR RGB(100, 180, 100)    // 绿色按钮

// 警告/提示函数：会阻塞直到用户点击 "确定"
void textWindow(const std::string& message) {
    // 自动获取屏幕/窗口中心坐标
    int screenW = SCREEN_WIDTH;
    int screenH = SCREEN_HEIGHT;
    
    int modalX = (screenW - MODAL_WIDTH) / 2;
    int modalY = (screenH - MODAL_HEIGHT) / 2;
    
    // 创建一个确认按钮
    int btnW = 100;
    int btnH = 40;
    // 按钮位置: 模态框右下角
    int btnX = modalX + MODAL_WIDTH - btnW - 20;
    int btnY = modalY + MODAL_HEIGHT - btnH - 15;

    Button confirmButton(btnX, btnY, btnW, btnH, "确定", BUTTON_COLOR);

    // 开启批量绘图，确保警告框一次性弹出
    BeginBatchDraw();

    bool exitModal = false;

    while (!exitModal) {
        // --- 绘制覆盖层（可选，用于半透明地遮挡底层画面）---
        // 不绘制半透明层，直接在现有画面上画，EasyX 效果最好
        
        // --- 绘制模态框主体 ---
        // 绘制阴影和边框
        setfillcolor(RGB(100, 100, 100));
        solidroundrect(modalX + 5, modalY + 5, modalX + MODAL_WIDTH + 5, modalY + MODAL_HEIGHT + 5, 8, 8); // 简单的阴影

        // 绘制主体背景
        setfillcolor(MODAL_BG_COLOR);
        setlinecolor(MODAL_BORDER_COLOR);
        setlinewidth(2);
        roundrect(modalX, modalY, modalX + MODAL_WIDTH, modalY + MODAL_HEIGHT, 8, 8);
        setlinewidth(1); // 恢复默认线宽

        // --- 绘制提示文字 ---
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        
        // 居中绘制警告信息
        int textW = textwidth((LPCTSTR)message.c_str());
        int textH = textheight((LPCTSTR)message.c_str());
        // 放在模态框上半部分居中
        outtextxy(modalX + (MODAL_WIDTH - textW) / 2, modalY + 50, (LPCTSTR)message.c_str());

        // --- 绘制按钮 ---
        confirmButton.draw();

        FlushBatchDraw();

        // --- 输入检测 (阻塞等待点击) ---
        ExMessage msg;
        // 使用 getmessage() 阻塞等待鼠标消息，直到用户操作
        msg = getmessage(EM_MOUSE | EM_KEY); 
        
        if (msg.message == WM_LBUTTONDOWN) {
            // 检查是否点击了 "确定" 按钮
            if (confirmButton.isClicked(msg.x, msg.y)) {
                exitModal = true;
            }
        }
    }

    // 结束批量绘图
    EndBatchDraw();
    
    // 警告框退出后，需要让调用者（PlayerLoop）在下一帧重绘底层画面
    // 这里不需要重新绘制 PlayerPanel，只需要结束模态框循环。
}