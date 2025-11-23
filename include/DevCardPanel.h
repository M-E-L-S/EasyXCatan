#pragma once
#include <vector>
#include <string>
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

private:
    bool visible = false;
    int panelX, panelY;

    std::vector<DevCardDisplay> cardDisplays;

    DevCardType selectedCardType = (DevCardType)-1;

    Button confirmButton;
    Button cancelButton;

    void drawCard(const DevCardDisplay& d, const DevCardManager& manager, const Player& player);
    void setupConfirmButtons(int x, int y);
};
