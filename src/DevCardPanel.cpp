#include "DevCardPanel.h"
#include "DevCardManager.h"
#include <graphics.h>
#include <string>

#define BTN_W 110
#define BTN_H 110
#define GAP   225

// [统一] 定义一个统一的深棕色用于启用状态
#define DEEP_BROWN RGB(160, 100, 50)
// 【新增】定义高亮颜色
#define HIGHLIGHT_COLOR RGB(255, 255, 150) // 亮黄色用于悬停边框
#define SELECTED_COLOR  RGB(255, 140, 0)   // 橙色用于选中边框


DevCardPanel::DevCardPanel(int startX, int startY)
    : panelX(startX), panelY(startY),
    hasBackgroundLoaded(false),
    confirmButton(0, 0, 80, 35, "confirm", RGB(50, 200, 50)),
    cancelButton(0, 0, 80, 35, "cancel", RGB(200, 50, 50)),
    backButton(0, 0, BTN_W, BTN_H, "BACK", RGB(150, 150, 150))
{
    // --- 卡牌按钮初始化逻辑 ---
    struct CardInfo {
        DevCardType t;
        const char* name;
        COLORREF color;
        bool hasButton;
    };

    CardInfo infos[] = {
            { KNIGHT,         "USE",      RGB(195,171,140),   true },
            { ROAD_BUILDING,  "USE",      RGB(195,171,140),   true },
            { YEAR_OF_PLENTY, "USE",      RGB(195,171,140),   true },
            { MONOPOLY,       "USE",      RGB(195,171,140),   true },
            { VICTORY_POINT,  "USE",    RGB(195,171,140),  false }
    };

    for (int i = 0; i < 5; i++) {
        auto& info = infos[i];
        DevCardDisplay d;

        d.type = info.t;
        d.name = info.name;

        int currentX = panelX + i * (BTN_W + GAP);
        int currentY = panelY;

        if (info.hasButton) {
            d.useButton = Button(currentX, currentY + 25, BTN_W, BTN_H, d.name.c_str(), info.color);
        }
        else {
            d.useButton = Button();
        }

        d.countX = currentX + 20;
        d.countY = currentY;

        cardDisplays.push_back(d);
    }

    int backBtnX = 1300; // 假设位置
    int backBtnY = panelY + 25; // 假设位置

    backButton.x = backBtnX;
    backButton.y = backBtnY;
    backButton.w = BTN_W;
    backButton.h = BTN_H;
    backButton.text = "BACK";
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

        if (canPlay) {
            d.useButton.color = DEEP_BROWN;
        }
    }

    backButton.color = DEEP_BROWN;
    backButton.enabled = true;
}


void DevCardPanel::drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player, int mouseX, int mouseY)
{
    if (d.type == VICTORY_POINT) {
        return;
    }

    // ... Draw logic ...
    d.useButton.draw();

    int btnX = d.useButton.x;
    int btnY = d.useButton.y;
    int btnW = d.useButton.w;
    int btnH = d.useButton.h;

    // --- 1. 悬停高亮逻辑：只有在未弹出确认框时才启用 ---
    if (selectedCardType == (DevCardType)-1) {

        bool isHover = (mouseX >= btnX && mouseX <= btnX + btnW &&
            mouseY >= btnY && mouseY <= btnY + btnH);

        // 如果悬停且按钮可用，绘制高亮效果
        if (isHover && d.useButton.enabled) {
            setfillcolor(0x33FFFFFF);
            solidrectangle(btnX, btnY, btnX + btnW, btnY + btnH);

            setlinecolor(HIGHLIGHT_COLOR);
            setlinestyle(PS_SOLID, 3);
            rectangle(btnX, btnY, btnX + btnW, btnY + btnH);
        }
    }

    // --- 2. 选中常亮边框逻辑：卡牌被选中时 (确认框弹出时) ---
    if (selectedCardType == d.type) {

        setlinecolor(SELECTED_COLOR);
        setlinestyle(PS_SOLID, 5);
        rectangle(btnX - 2, btnY - 2, btnX + btnW + 2, btnY + btnH + 2);
    }
    setlinestyle(PS_SOLID, 1); // 恢复默认线型
}

void DevCardPanel::setupConfirmButtons(int x, int y)
{
    confirmButton.x = x;
    confirmButton.y = y;
    cancelButton.x = x + confirmButton.w + 15;
    cancelButton.y = y;
    confirmButton.text = "confirm";
    cancelButton.text = "cancel";
}


void DevCardPanel::draw(const DevCardManager& manager, const Player& player, int mouseX, int mouseY)
{
    if (!visible) return;

    if (hasBackgroundLoaded) {
        putimage(0, 0, &backgroundImage);
    }

    for (auto& d : cardDisplays)
        drawCard(d, manager, player, mouseX, mouseY);

    backButton.draw();

    // 绘制卡牌确认/取消浮动框
    if (selectedCardType != (DevCardType)-1) {

        int X = getwidth() / 2 - 130;
        int Y = getheight() / 2 - 55;

        setfillcolor(RGB(40, 40, 40));
        bar(X, Y, X + 260, Y + 110);

        setcolor(WHITE);
        settextstyle(18, 0, _T("Arial Black"));
        outtextxy(X + 20, Y + 15, "Are you sure you want to use this card?");

        setupConfirmButtons(X + 25, Y + 55);
        confirmButton.draw();
        cancelButton.draw();
    }
}

int DevCardPanel::handleClick(int mx, int my)
{
    if (!visible) return -1;

    // 1. 如果确认框已经弹出 (优先级最高)
    if (selectedCardType != (DevCardType)-1) {
        if (confirmButton.isClicked(mx, my)) {
            DevCardType t = selectedCardType;
            selectedCardType = (DevCardType)-1;
            return (int)t; // 直接返回卡牌类型
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
            selectedCardType = d.type; // 设置选中类型
            return -1; // 停留在 loop，等待确认/取消
        }
    }

    // 3. 返回按钮
    if (backButton.isClicked(mx, my)) {
        return -2;  // 用户点击返回
    }

    return -1;
}


void DevCardPanel::loadBackgroundImage(const char* filePath, int optionalWidth, int optionalHeight) {
    if (optionalWidth > 0 && optionalHeight > 0) {
        loadimage(&backgroundImage, filePath, optionalWidth, optionalHeight);
    }
    else {
        loadimage(&backgroundImage, filePath);
    }
    hasBackgroundLoaded = true;
}