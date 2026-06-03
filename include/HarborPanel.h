#pragma once
#include <vector>
#include <graphics.h>
#include "Common.h"
#include "Player.h"
#include "Button.h"

class HarborPanel {
public:
    // 新增 tradeOptions 参数
    HarborPanel(int screenWidth, int screenHeight, const std::vector<bool>& tradeOptions);

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
    int selectedGiveType;              // 选中的支出资源 (-1 表示未选)
    int getQuantities[RESOURCE_COUNT]; // 选中的获得资源数量

    // 图片资源
    IMAGE resourceImages[RESOURCE_COUNT];
    bool hasImagesLoaded;

    // 【新增】背景图片资源
    IMAGE bgImage;
    bool hasBgLoaded;

    Button* btnConfirm;
    Button* btnCancel;

    // 交易选项 (7 位)：[4:1, 3:1, 木材2:1, 砖块2:1, 矿石2:1, 羊毛2:1, 粮食2:1]
    std::vector<bool> tradeOptions;

    // 辅助函数
    int getResourceAtClick(int mouseX, int mouseY, int uiOffsetX, int uiOffsetY);
    void drawArtisticText(int x, int y, const TCHAR* text, int size, COLORREF mainColor = RGB(192, 192, 192));

    // 根据传入的 tradeOptions 和资源类型，返回该资源的兑换率 (2/3/4)，若返回 0 则表示不可兑换
    int getRateForResource(int resourceIndex) const;
};