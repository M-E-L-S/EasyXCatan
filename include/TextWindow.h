#pragma once

#include <graphics.h>
#include <string>
#include "Button.h" // 依赖你提供的 Button.h 文件
#include "Common.h"

// 定义模态框的颜色和尺寸
#define MODAL_WIDTH 400
#define MODAL_HEIGHT 200
#define MODAL_BG_COLOR RGB(195,171,140) // 浅棕色背景
#define MODAL_BORDER_COLOR RGB(50, 50, 50)  // 深色边框
#define BUTTON_COLOR RGB(194,37,37)    // 红色按钮

// 警告/提示函数：会阻塞直到用户点击 "确定"
void textWindow(const std::string& message) ;