#include "DevCardPanel.h"
#include "DevCardManager.h"
#include <graphics.h>
#include <string>

#define BTN_W 360
#define BTN_H 600
#define GAP   20

DevCardPanel::DevCardPanel(int startX, int startY)
    : panelX(startX), panelY(startY),
    confirmButton(0, 0, 80, 35, "确认", RGB(50, 200, 50)),
    cancelButton(0, 0, 80, 35, "取消", RGB(200, 50, 50))
{
    struct CardInfo {
        DevCardType t;
        const char* name;
        COLORREF color;
        bool hasButton;
    };

    CardInfo infos[] = {
        { KNIGHT,         "骑士",      RGB(200,180,80),   true },
        { ROAD_BUILDING,  "道路建设",  RGB(180,200,80),   true },
        { YEAR_OF_PLENTY, "丰收之年",  RGB(80,200,120),   true },
        { MONOPOLY,       "垄断",      RGB(200,80,120),   true },
        { VICTORY_POINT,  "胜利点",    RGB(120,120,220),  false }
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
}

void DevCardPanel::update(const Player& player, const DevCardManager& manager)
{
    for (auto& d : cardDisplays) {
        d.count = player.getDevCardCount(d.type);

        if (d.type == VICTORY_POINT) {
            d.useButton.enabled = false;
            continue;
        }

        d.useButton.enabled = (d.count > 0) && manager.CanPlayCard(player.getID(), d.type);
    }
}

void DevCardPanel::drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player)
{
    settextstyle(16, 0, _T("宋体"));
    setcolor(WHITE);

    std::string s = d.name + (" x" + std::to_string(d.count));
    outtextxy(d.countX, d.countY, s.c_str());

    if (d.type != VICTORY_POINT && selectedCardType == (DevCardType)-1)
        d.useButton.draw();

    // ----------------- 绘制总数和可用数量框 -----------------
    int boxX = d.useButton.x;
    int boxY = d.useButton.y + d.useButton.h + 5;
    int boxW = d.useButton.w;
    int boxH = 55;

    setfillcolor(RGB(60, 60, 60));
    bar(boxX, boxY, boxX + boxW, boxY + boxH);
    setlinecolor(WHITE);
    rectangle(boxX, boxY, boxX + boxW, boxY + boxH);

    // 获取可用数量
    int total = player.getDevCardCount(d.type);
    int usedThisTurn = manager.getNewThisTurn(player.getID(), d.type);
    int usable = total - usedThisTurn;

    std::string info = "总数: " + std::to_string(total) + ", 可用: " + std::to_string(usable);
    setcolor(WHITE);
    outtextxy(boxX + 5, boxY + 5, info.c_str());

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

    for (auto& d : cardDisplays)
        drawCard(d, manager, player);

    if (selectedCardType != (DevCardType)-1) {
        int X = panelX + 625;
        int Y = panelY + 320;

        setfillcolor(RGB(40, 40, 40));
        bar(X, Y, X + 260, Y + 110);

        setcolor(WHITE);
        settextstyle(18, 0, _T("宋体"));
        outtextxy(X + 20, Y + 15, "确定要使用这张卡？");

        setupConfirmButtons(X + 25, Y + 55);
        confirmButton.draw();
        cancelButton.draw();
    }
}

int DevCardPanel::handleClick(int mx, int my)
{
    if (!visible) return -1;

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

    for (auto& d : cardDisplays) {
        if (d.useButton.enabled && d.useButton.isClicked(mx, my)) {
            selectedCardType = d.type;
            return -1;
        }
    }

    return -1;
}