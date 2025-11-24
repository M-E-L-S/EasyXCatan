#pragma once
#include <vector>
#include <string>
#include <graphics.h> // 确保包含 easyx 图形库头文件
#include "Common.h"   // 假设 Common.h 定义了 DevCardType 等枚举
#include "Player.h"
#include "Button.h"   // 假设 Button 类已定义

struct DevCardDisplay {
    DevCardType type;
    std::string name;
    Button useButton;
    int count = 0;

    int countX = 0;
    int countY = 0;
};

class DevCardManager;

class DevCardPanel {
public:
    DevCardPanel(int startX, int startY);

    void setVisible(bool v) { visible = v; }

    void update(const Player& player, const DevCardManager& manager);

    // 【关键修改】 draw 函数签名更新，接受 mouseX, mouseY
    void draw(const DevCardManager& manager, const Player& player, int mouseX, int mouseY);

    int handleClick(int mx, int my);

    // 【关键修改】 loadBackgroundImage 声明，解决链接错误
    void loadBackgroundImage(const char* filePath, int optionalWidth = 0, int optionalHeight = 0);

private:
    bool visible = false;
    int panelX, panelY;

    // 背景图相关变量
    IMAGE backgroundImage;
    bool hasBackgroundLoaded = false;

    std::vector<DevCardDisplay> cardDisplays;

    DevCardType selectedCardType = (DevCardType)-1;

    Button confirmButton;
    Button cancelButton;
    Button backButton;

    // 【关键修改】 drawCard 函数签名更新，接受 mouseX, mouseY
    void drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player, int mouseX, int mouseY);
    void setupConfirmButtons(int x, int y);
};