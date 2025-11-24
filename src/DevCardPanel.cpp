#include "DevCardPanel.h"

#include "DevCardManager.h"
#include <graphics.h>
#include <string>

#define BTN_W 110
#define BTN_H 110
#define GAP   225

// [新增/统一] 定义一个统一的深棕色用于启用状态
#define DEEP_BROWN RGB(160, 100, 50)

DevCardPanel::DevCardPanel(int startX, int startY)
    : panelX(startX), panelY(startY),
    hasBackgroundLoaded(false), // [新增] 初始化
    confirmButton(0, 0, 80, 35, "confirm", RGB(50, 200, 50)),
    cancelButton(0, 0, 80, 35, "cancel", RGB(200, 50, 50)),
    // [新增] 返回按钮的初始化 (初始位置不重要，后面会设置)
    backButton(0, 0, BTN_W, BTN_H, "back", RGB(150, 150, 150))
{
    struct CardInfo {
        DevCardType t;
        const char* name;
        COLORREF color;
        bool hasButton;
    };

    CardInfo infos[] = {
            { KNIGHT,         "USE",      RGB(195,171,140),   true },
            { ROAD_BUILDING,  "USE",  RGB(195,171,140),   true },
            { YEAR_OF_PLENTY, "USE",  RGB(195,171,140),   true },
            { MONOPOLY,       "USE",     RGB(195,171,140),   true },
            { VICTORY_POINT,  "USE",     RGB(195,171,140),  false }
    };

    for (int i = 0; i < 5; i++) {
        auto& info = infos[i];
        DevCardDisplay d;

        d.type = info.t;
        d.name = info.name;

        int currentX = panelX + i * (BTN_W + GAP);
        int currentY = panelY;

        if (info.hasButton) {
            d.useButton = Button(currentX, currentY + 25, BTN_W, BTN_H, d.name, info.color);
        }
        else {
            d.useButton = Button();  // 空按钮
        }

        d.countX = currentX + 20;
        d.countY = currentY;

        cardDisplays.push_back(d);
    }

    // ==========================================================
    // [新增] 初始化返回按钮的位置和属性
    // ==========================================================

    // 现有 5 张卡牌 (i=0到4)。返回按钮放在 i=5 的位置
    int backBtnIndex = 5;

    // 临时固定在一个较小的 X 坐标，例如 1000，确保可见。
    int backBtnX = 1300;

    // Y 坐标保持与卡牌按钮对齐
    int backBtnY = panelY + 25;

    backButton.x = backBtnX;
    backButton.y = backBtnY;
    backButton.w = BTN_W;
    backButton.h = BTN_H;
    backButton.text = "Back";
    backButton.color = RGB(100, 100, 100);
    backButton.enabled = true;
}

void DevCardPanel::update(const Player& player, const DevCardManager& manager)
{
    for (auto& d : cardDisplays) {
        d.count = player.getDevCardCount(d.type);

        if (d.type == VICTORY_POINT) {
            d.useButton.enabled = false;
            continue;
        }

        bool canPlay = (d.count > 0) && manager.CanPlayCard(player.getID(), d.type);
        d.useButton.enabled = canPlay;

        // [统一颜色] 可用时设为棕色，不可用时按钮将显示其默认的 disabled 颜色
        if (canPlay) {
            d.useButton.color = DEEP_BROWN;
        }
    }

    // [统一颜色] 返回按钮统一设置为棕色
    backButton.color = DEEP_BROWN;
    backButton.enabled = true;
}

void DevCardPanel::drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player)
{
    if (d.type != VICTORY_POINT && selectedCardType == (DevCardType)-1)
        d.useButton.draw();
}

void DevCardPanel::setupConfirmButtons(int x, int y)
{
    confirmButton.x = x;
    confirmButton.y = y;
    cancelButton.x = x + confirmButton.w + 15;
    cancelButton.y = y;
}

void DevCardPanel::draw(const DevCardManager& manager, const Player& player)
{
    if (!visible) return;

    if (hasBackgroundLoaded) {
        putimage(0, 0, &backgroundImage);
    }

    for (auto& d : cardDisplays)
        drawCard(d, manager, player);

    // ==========================================================
    // [新增] 绘制返回按钮
    // ==========================================================
    if (selectedCardType == (DevCardType)-1) {
        backButton.draw();
    }

    // 绘制确认/取消浮动框
    if (selectedCardType != (DevCardType)-1) {
        int X = getwidth() / 2 - 130;
        int Y = getheight() / 2 - 55;

        setfillcolor(RGB(40, 40, 40));
        bar(X, Y, X + 260, Y + 110);

        setcolor(WHITE);
        settextstyle(18, 0, _T("宋体"));
        outtextxy(X + 20, Y + 15, "确认使用这张卡牌吗？");

        setupConfirmButtons(X + 25, Y + 55);
        confirmButton.draw();
        cancelButton.draw();
    }
}

int DevCardPanel::handleClick(int mx, int my)
{
    if (!visible) return -1;

    // 1. 如果确认框已经弹出
    if (selectedCardType != (DevCardType)-1) {
        if (confirmButton.isClicked(mx, my)) {
            DevCardType t = selectedCardType;
            selectedCardType = (DevCardType)-1;
            return (int)t;
        }
        if (cancelButton.isClicked(mx, my)) {
            selectedCardType = (DevCardType)-1;
            return -1;
        }
        return -1;
    }

    // 2. 正常模式，检测卡牌
    for (auto& d : cardDisplays) {
        if (d.useButton.enabled && d.useButton.isClicked(mx, my)) {
            selectedCardType = d.type;
            return -1;
        }
    }

    // ==========================================================
    // [新增] 返回按钮
    // ==========================================================
    if (backButton.isClicked(mx, my)) {
        return -2;  // 用户点击返回
    }

    return -1;
}

// [新增] 加载背景图片实现
void DevCardPanel::loadBackgroundImage(const char* filePath, int optionalWidth, int optionalHeight) {
    if (optionalWidth > 0 && optionalHeight > 0) {
        loadimage(&backgroundImage, filePath, optionalWidth, optionalHeight);
    }
    else {
        loadimage(&backgroundImage, filePath);
    }
    hasBackgroundLoaded = true;
}
