#pragma once
// DevCardPanel.h
#pragma once
#include <vector>
#include <string>
#include <graphics.h> // 确保包含 easyx 图形库头文件
#include "Common.h"
#include "Player.h"
#include "Button.h"

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

    void draw(const DevCardManager& manager, const Player& player);

    int handleClick(int mx, int my);

    // [新增] 加载背景图片的函数声明
    // optionalWidth 和 optionalHeight 如果传入 > 0，则会自动缩放图片
    void loadBackgroundImage(const char* filePath, int optionalWidth = 0, int optionalHeight = 0);

private:
    bool visible = false;
    int panelX, panelY;

    // [新增] 背景图相关变量
    IMAGE backgroundImage;
    bool hasBackgroundLoaded = false;

    std::vector<DevCardDisplay> cardDisplays;

    DevCardType selectedCardType = (DevCardType)-1;

    Button confirmButton;
    Button cancelButton;

    void drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player);
    void setupConfirmButtons(int x, int y);
};