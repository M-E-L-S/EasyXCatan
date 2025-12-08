// RobbedResourcePanel.h
#pragma once
#include "Common.h"
#include "Button.h"
#include <graphics.h>
#include <string>

class RobbedResourcePanel {
private:
    // 核心成员变量
    int width, height;               // 窗口宽高
    int stolenResType;               // 抢夺到的资源类型（-1=扑空，0-4=对应资源）
    int victimID;                    // 被抢夺者ID（P+ID）
    IMAGE resourceImages[RESOURCE_COUNT]; // 资源图片
    bool hasImagesLoaded;            // 是否加载图片
    IMAGE fullScreenBgImage;  // 全屏背景图
    bool hasFullScreenBgLoaded;  // 是否加载成功

    // 新UI布局常量（完全沿用目标UI风格）
    const int DIALOG_W = 600;        // 弹窗宽度
    const int DIALOG_H = 320;        // 弹窗高度
    const int RES_ICON_SIZE_PANEL = 100; // 资源图标大小（和目标UI一致）
    const int BUTTON_W = 160;        // 按钮宽度
    const int BUTTON_H = 45;         // 按钮高度
    int START_X;                     // 弹窗起始X（水平居中）
    int START_Y;                     // 弹窗起始Y（垂直居中）

    // Button对象（仅用现有接口，不修改Button类）
    Button confirmBtn;               // 确认按钮（蓝色，目标UI样式）

    // 资源颜色数组（和目标UI一致）
    const COLORREF resColors[RESOURCE_COUNT] = {
        RGB(34, 139, 34),    // 木头
        RGB(178, 34, 34),    // 砖块
        RGB(152, 251, 152),  // 羊毛
        RGB(255, 215, 0),    // 小麦
        RGB(105, 105, 105)   // 矿石
    };

    // 辅助函数（绘制各组件）
    void calculateLayout();    // 计算所有元素居中布局
    void drawMask();           // 绘制深色遮罩
    void drawDialog();         // 绘制弹窗主体（背景+边框）
    void drawTexts();          // 绘制标题+结果文字
    void drawResourceIcon();   // 绘制抢夺到的资源图标/色块（目标UI核心）
    void drawButton();         // 绘制确认按钮

public:
    // 构造函数：传入窗口宽高+抢夺结果（适配目标UI场景）
    // stolenResType: -1=扑空，0-4=对应资源类型；victimID: 被抢夺者ID
    RobbedResourcePanel(int w, int h, int stolenResType, int victimID);

    // 加载资源图片（可选，无图显示色块）
    void loadResourceImages(const char* paths[RESOURCE_COUNT]);

    void loadFullScreenBackground(const char* bgPath);

    // 显示面板（阻塞式，点击确认关闭）
    void show();
};