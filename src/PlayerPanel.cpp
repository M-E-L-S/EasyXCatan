
// PlayerPanel.cpp
#include "PlayerPanel.h"
#include "Common.h"
#include <string>

// --- 布局常量 ---

// 第1栏: 玩家信息 & 资源
const int COL1_X = 150;
const int INFO_Y = 10;
const int RES_Y = 50;     // 资源图片起始Y坐标
const int RES_ICON_SIZE = 130; // 假设资源图片显示大小为 200x200
const int RES_GAP = 20;    // 资源之间的间距


// 第2栏: 核心动作
const int COL2_X = 720;
const int BUILD_Y = 150;   // 建造按钮起始Y
const int TRADE_Y = 400;   // 交易/卡牌按钮起始Y

// 第3栏: 游戏流程 & 状态
const int COL3_X = 1200;
const int STATUS_Y = 50;   // 状态文字
const int DEV_Y = 300;     // 发展卡文字
const int FLOW_Y = 600;    // 流程按钮起始Y

// 按钮大小
const int BTN_W = 200;
const int BTN_H = 50;
const int BTN_GAP = 10;    // 按钮垂直间距

PlayerPanel::PlayerPanel(int screenWidth, int screenHeight)
    : width(screenWidth), height(screenHeight), hasImagesLoaded(false)
{
    // 1. 安全预留空间，防止 vector 扩容导致野指针
    buttons.reserve(30);

    // --- 第2栏: 建造按钮组 ---
    int currentY = BUILD_Y;

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Build Road", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_ROAD] = &buttons.back();
    currentY += (BTN_H + BTN_GAP);

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Build Settlement", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_SETTLEMENT] = &buttons.back();
    currentY += (BTN_H + BTN_GAP);

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Build City", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_CITY] = &buttons.back();

    // --- 第2栏: 购买/交易按钮组 ---
    currentY = TRADE_Y;

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Buy Dev Card", GREEN));
    buttonMap[ButtonTypeFromPanel::BUY_DEV_CARD] = &buttons.back();
    currentY += (BTN_H + BTN_GAP);

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Use Dev Card", GREEN));
    buttonMap[ButtonTypeFromPanel::USE_DEV_CARD] = &buttons.back();
    currentY += (BTN_H + BTN_GAP);

    buttons.push_back(Button(COL2_X, currentY, BTN_W, BTN_H, "Trade (Bank 4:1)", GREEN));
    buttonMap[ButtonTypeFromPanel::TRADE_BANK] = &buttons.back();

    // --- 第3栏: 游戏流程按钮 ---
    currentY = FLOW_Y;

    buttons.push_back(Button(COL3_X, currentY, BTN_W, BTN_H, "Switch To Map", BLUE));
    buttonMap[ButtonTypeFromPanel::SWITCH_TO_MAP] = &buttons.back();
    currentY += (BTN_H + BTN_GAP);

    buttons.push_back(Button(COL3_X, currentY, BTN_W, BTN_H, "END TURN", RED));
    buttonMap[ButtonTypeFromPanel::END_TURN] = &buttons.back();
}

// --- 加载图片的接口 ---
void PlayerPanel::loadResourceImages(const char* filePaths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        // loadimage 是 EasyX 的函数
        // 参数: &image对象, 路径, 宽, 高 (设置为 RES_ICON_SIZE 自动缩放)
        // 注意：如果路径不对，EasyX 可能不会报错但在控制台会有提示，图片会是黑的
        loadimage(&resourceImages[i], filePaths[i], RES_ICON_SIZE, RES_ICON_SIZE);
    }
    hasImagesLoaded = true;
}

// --- 绘制主函数 ---
void PlayerPanel::draw(const Player& player, GameState state, int lastDiceResult) {
    // [修复] 使用 RGB 而不是 EGERGB
    setfillcolor(RGB(240, 248, 255)); // Alice Blue
    solidrectangle(0, 0, width, height);

    // 2. 绘制各栏内容
    drawPlayerInfo(player);
    drawResources(player);
    drawDevCards(player);     // 显示在第一栏下方
    drawStatusMessage(state, lastDiceResult); // 显示在第三栏上方

    // 3. 绘制按钮 (逻辑状态更新)
    drawButtons(player, state);
}

ButtonTypeFromPanel PlayerPanel::getClickedButton(int mouseX, int mouseY) {
    for (auto const& pair : buttonMap) {
        if (pair.second->isClicked(mouseX, mouseY)) {
            return pair.first;
        }
    }
    return ButtonTypeFromPanel::NONE;
}

// --- 私有绘制函数 ---

void PlayerPanel::drawPlayerInfo(const Player& player) {
    setbkmode(TRANSPARENT);
    settextcolor(player.getColor());
    // 注意：Arial Black 字体如果系统没有，会回退到默认字体
    settextstyle(30, 0, _T("微软雅黑"));

    std::string p_info = "Player " + std::to_string(player.getID());
    outtextxy(COL1_X, INFO_Y, p_info.c_str());
}

void PlayerPanel::drawResources(const Player& player) {
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, _T("Arial"));
    settextcolor(BLACK);

    // 资源名称备用（如果没有图片）
    const char* resNames[RESOURCE_COUNT] = { "Wood", "Brick", "Sheep", "Wheat", "Ore" };
    // 资源对应颜色备用
    COLORREF resColors[RESOURCE_COUNT] = { RGB(34,139,34), RGB(178,34,34), RGB(152,251,152), RGB(255,215,0), RGB(105,105,105) };

    int currentY = RES_Y;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        ResourceType type = (ResourceType)i;
        int count = player.getResourceCount(type);

        if (hasImagesLoaded) {
            // 1. 绘制图片
            putimage(COL1_X, currentY, &resourceImages[i]);

            // 2. 绘制数量 (显示在图片右侧)
            std::string countStr = "x " + std::to_string(count) + "   " + resNames[i];

            // 垂直居中对齐文字
            int textY = currentY + (RES_ICON_SIZE - 20) / 2;
            outtextxy(COL1_X + RES_ICON_SIZE + 10, textY, countStr.c_str());
        }
        else {
            // 如果还没加载图片，画一个色块代替
            setfillcolor(resColors[i]);
            solidrectangle(COL1_X, currentY, COL1_X + RES_ICON_SIZE, currentY + RES_ICON_SIZE);

            std::string info = std::string(resNames[i]) + ": " + std::to_string(count);
            outtextxy(COL1_X + RES_ICON_SIZE + 10, currentY + 10, info.c_str());
        }

        currentY += (RES_ICON_SIZE + RES_GAP);
    }

    // 显示总数
    std::string total = "Total Cards: " + std::to_string(player.getTotalResourceCount());
    outtextxy(COL1_X, currentY + 10, total.c_str());
}

void PlayerPanel::drawDevCards(const Player& player) {
    int startY = DEV_Y;

    setbkmode(TRANSPARENT);
    settextstyle(18, 0, _T("Arial"));
    settextcolor(BLACK);

    outtextxy(COL3_X, startY - 25, "Dev Cards:");

    const char* devCardNames[DEV_CARD_COUNT] = {
        "Knight", "VP", "Road Build", "Monopoly", "Plenty"
    };

    for (int i = 0; i < DEV_CARD_COUNT; i++) {
        DevCardType type = (DevCardType)i;
        int count = player.getDevCardCount(type);
        std::string str = std::string(devCardNames[i]) + ": " + std::to_string(count);

        outtextxy(COL3_X, startY + (i * 25), str.c_str());
    }
}

void PlayerPanel::drawStatusMessage(GameState state, int lastDiceResult) {
    setbkmode(TRANSPARENT);
    settextcolor(BLUE);
    settextstyle(22, 0, _T("Arial Black"));

    // 显示骰子结果
    if (lastDiceResult > 0) {
        std::string diceMsg = "Dice Roll: " + std::to_string(lastDiceResult);
        outtextxy(COL3_X, STATUS_Y, diceMsg.c_str());
    }
    else {
        outtextxy(COL3_X, STATUS_Y, "Waiting for turn...");
    }

    // 显示状态提示
    settextstyle(18, 0, _T("Arial"));
    settextcolor(BLACK);

    RECT r = { COL3_X, STATUS_Y + 40, COL3_X + 250, STATUS_Y + 200 };
    std::string msg;

    switch (state) {
    case GameState::IDLE:
        msg = "Choose an action from the middle panel.";
        break;
    case GameState::AWAITING_MAP_CLICK_FOR_SETTLEMENT:
        msg = "Click 'Switch To Map' and place a Settlement.";
        break;
    case GameState::AWAITING_MAP_CLICK_FOR_ROAD:
        msg = "Click 'Switch To Map' and place a Road.";
        break;
    case GameState::AWAITING_MAP_CLICK_FOR_CITY:
        msg = "Click 'Switch To Map' and upgrade a Settlement.";
        break;
    default:
        msg = "Current State: " + std::to_string((int)state);
        break;
    }

    // 注意：如果你的项目是 Unicode 字符集，这里的 msg.c_str() (char*) 会报错
    // 解决方法：在项目属性中设置字符集为 "使用多字节字符集"
    drawtext(msg.c_str(), &r, DT_LEFT | DT_WORDBREAK);
}

void PlayerPanel::drawButtons(const Player& player, GameState state) {

    // 1. IDLE 状态下根据资源判断是否启用
    if (state == GameState::IDLE) {
        // 建造类
        if (buttonMap.count(ButtonTypeFromPanel::BUILD_ROAD))
            buttonMap[ButtonTypeFromPanel::BUILD_ROAD]->enabled = player.canAffordBuilding(ROAD);

        if (buttonMap.count(ButtonTypeFromPanel::BUILD_SETTLEMENT))
            buttonMap[ButtonTypeFromPanel::BUILD_SETTLEMENT]->enabled = player.canAffordBuilding(SETTLEMENT);

        if (buttonMap.count(ButtonTypeFromPanel::BUILD_CITY))
            buttonMap[ButtonTypeFromPanel::BUILD_CITY]->enabled = player.canAffordBuilding(CITY);

        if (buttonMap.count(ButtonTypeFromPanel::BUY_DEV_CARD))
            buttonMap[ButtonTypeFromPanel::BUY_DEV_CARD]->enabled = player.canAffordDevCard();

        // 功能类 (总是启用)
        if (buttonMap.count(ButtonTypeFromPanel::USE_DEV_CARD))
            buttonMap[ButtonTypeFromPanel::USE_DEV_CARD]->enabled = true;

        if (buttonMap.count(ButtonTypeFromPanel::TRADE_BANK))
            buttonMap[ButtonTypeFromPanel::TRADE_BANK]->enabled = true;

        if (buttonMap.count(ButtonTypeFromPanel::END_TURN))
            buttonMap[ButtonTypeFromPanel::END_TURN]->enabled = true;

    }
    else {
        // 非 IDLE 状态 (如等待地图点击)，禁用大部分按钮
        for (auto& pair : buttonMap) {
            pair.second->enabled = false;
        }
    }

    // "Switch To Map" 总是启用，方便查看地图
    if (buttonMap.count(ButtonTypeFromPanel::SWITCH_TO_MAP))
        buttonMap[ButtonTypeFromPanel::SWITCH_TO_MAP]->enabled = true;

    // 绘制所有按钮
    for (auto const& pair : buttonMap) {
        pair.second->draw();
    }
}