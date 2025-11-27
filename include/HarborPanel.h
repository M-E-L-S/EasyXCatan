#pragma once
#include <vector>
#include <graphics.h>
#include "Common.h"
#include "Player.h"
#include "Button.h"

class HarborPanel {
public:
    HarborPanel(int screenWidth, int screenHeight);

    // 加载资源图标 (木、砖、羊、麦、矿)
    void loadResourceImages(const char* filePaths[RESOURCE_COUNT]);

    // 【新增】加载银行背景图片
    // 请在初始化时调用：bank.loadBackgroundImage("assets/bank_bg.png");
    void loadBackgroundImage(const char* filePath);

    // 绘制函数
    void draw(const Player& player, int mouseX, int mouseY);

    // 输入处理
    bool handleInput(ExMessage& msg, Player& player);

private:
    int width, height;

    // 状态变量
    int selectedGiveType;              // 选中的支出资源
    int getQuantities[RESOURCE_COUNT]; // 选中的获得资源数量

    // 图片资源
    IMAGE resourceImages[RESOURCE_COUNT];
    bool hasImagesLoaded;

    // 【新增】背景图片资源
    IMAGE bgImage;
    bool hasBgLoaded;

    Button* btnConfirm;
    Button* btnCancel;

    // 辅助函数
    int getResourceAtClick(int mouseX, int mouseY, int uiOffsetX, int uiOffsetY);
    void drawArtisticText(int x, int y, const TCHAR* text, int size, COLORREF mainColor = RGB(192, 192, 192));
};