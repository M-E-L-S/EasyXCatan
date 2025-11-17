// PlayerPanel.cpp
#include "PlayerPanel.h"
#include "Common.h"

// --- 布局常量 ---
// to consider
// 第1栏: 玩家信息
const int COL1_X = 50;
const int INFO_Y = 50;
const int RES_Y = 150;
const int DEV_Y = 400;

// 第2栏: 核心动作
const int COL2_X = 400;
const int BUILD_Y = 150;
const int TRADE_Y = 400;

// 第3栏: 游戏流程
const int COL3_X = 750;
const int STATUS_Y = 50;
const int FLOW_Y = 600;

// 按钮大小
const int BTN_W = 200;
const int BTN_H = 50;


PlayerPanel::PlayerPanel(int screenWidth, int screenHeight)
    : width(screenWidth), height(screenHeight)
{
    // --- 在这里初始化所有按钮的位置 ---

    // 第2栏: 建造
    buttons.push_back(Button(COL2_X, BUILD_Y,      BTN_W, BTN_H, "Build Road", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_ROAD] = &buttons.back();

    buttons.push_back(Button(COL2_X, BUILD_Y + 60, BTN_W, BTN_H, "Build Settlement", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_SETTLEMENT] = &buttons.back();

    buttons.push_back(Button(COL2_X, BUILD_Y + 120, BTN_W, BTN_H, "Build City", YELLOW));
    buttonMap[ButtonTypeFromPanel::BUILD_CITY] = &buttons.back();

    // 第2栏: 购买/交易
    buttons.push_back(Button(COL2_X, TRADE_Y,      BTN_W, BTN_H, "Buy Dev Card", GREEN));
    buttonMap[ButtonTypeFromPanel::BUY_DEV_CARD] = &buttons.back();

    buttons.push_back(Button(COL2_X, TRADE_Y + 60, BTN_W, BTN_H, "Use Dev Card", GREEN));
    buttonMap[ButtonTypeFromPanel::USE_DEV_CARD] = &buttons.back();

    buttons.push_back(Button(COL2_X, TRADE_Y + 120, BTN_W, BTN_H, "Trade (Bank 4:1)", GREEN));
    buttonMap[ButtonTypeFromPanel::TRADE_BANK] = &buttons.back();

    // 第3栏: 游戏流程
    buttons.push_back(Button(COL3_X, FLOW_Y,      BTN_W, BTN_H, "Switch To Map", BLUE));
    buttonMap[ButtonTypeFromPanel::SWITCH_TO_MAP] = &buttons.back();

    buttons.push_back(Button(COL3_X, FLOW_Y + 60, BTN_W, BTN_H, "END TURN", RED));
    buttonMap[ButtonTypeFromPanel::END_TURN] = &buttons.back();

    //这里还要加弃牌的按钮！！！！
    //这里要加丢色子的按钮

}

/*
 * @brief 绘制的主函数 (由 GameController 调用)
 */
void PlayerPanel::draw(const Player& player, GameState state) {
    // 1. 绘制背景
    setfillcolor(EGERGB(240, 248, 255)); // 淡淡的"爱丽丝蓝"
    solidrectangle(0, 0, width, height);

    // 2. 绘制玩家信息 (第1栏)
    drawPlayerInfo(player);
    drawResources(player);
    drawDevCards(player);

    // 3. 绘制状态信息 (第3栏)
    drawStatusMessage(state);

    // 4. 绘制所有按钮 (第2、3栏)
    // 这是核心：根据状态更新按钮的 "enabled" 属性
    drawButtons(player, state);
}

/*
 * @brief 检查点击 (由 GameController 调用)
 */
ButtonTypeFromPanel PlayerPanel::getClickedButton(int mouseX, int mouseY) {
    // 遍历我们的 map
    for (auto const& [type, button] : buttonMap) {
        if (button->isClicked(mouseX, mouseY)) {
            return type; // 找到了！返回按钮的类型
        }
    }
    return ButtonTypeFromPanel::NONE; // 什么都没点到
}


// --- 所有的私有绘制函数 ---

void PlayerPanel::drawPlayerInfo(const Player& player) {
    //playerID
    setbkmode(TRANSPARENT);//设置文字背景样式
    settextcolor(player.getColor());// 设置玩家颜色
    settextstyle(30, 0, _T("Arial"));

    std::string p_info = "Player " + std::to_string(player.getID());
    outtextxy(COL1_X, INFO_Y, p_info.c_str());

    //points
    settextcolor(BLACK); // 重置为黑色
    settextstyle(20, 0, _T("Arial"));
    std::string score = "Score: " + std::to_string(player.getVictoryPoints());
    outtextxy(COL1_X, INFO_Y + 40, score.c_str());
}

void PlayerPanel::drawResources(const Player& player) {
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, _T("Arial"));
    outtextxy(COL1_X, RES_Y - 30, "Resources:");

    // C-style 字符串数组
    const char* resNames[RESOURCE_COUNT] = {"Wood", "Brick", "Sheep", "Wheat", "Ore"};

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        ResourceType type = (ResourceType)i;
        std::string resStr = resNames[i] + (": " + std::to_string(player.getResourceCount(type)));
        outtextxy(COL1_X, RES_Y + (i * 30), resStr.c_str());
    }
    std::string total = "Total: " + std::to_string(player.getTotalResourceCount());
    outtextxy(COL1_X, RES_Y + (RESOURCE_COUNT * 30), total.c_str());
}

void PlayerPanel::drawDevCards(const Player& player) {
    // 设置标题“Development Cards:”
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, _T("Arial"));
    settextcolor(BLACK); // 确保文字颜色是黑色
    outtextxy(COL1_X, DEV_Y - 30, "Development Cards:");

    // 创建一个 C 风格字符串数组
    // *** 关键：这个数组的顺序必须与 DevCardType 枚举的顺序完全一致 ***
    const char* devCardNames[DEV_CARD_COUNT] = {
        "Knight",         // 对应 KNIGHT (索引 0)
        "Victory Point",  // 对应 VICTORY_POINT (索引 1)
        "Road Building",  // 对应 ROAD_BUILDING (索引 2)
        "Monopoly",       // 对应 MONOPOLY (索引 3)
        "Year of Plenty"  // 对应 YEAR_OF_PLENTY (索引 4)
    };

    // 循环遍历所有类型的发展卡 (从 0 到 DEV_CARD_COUNT - 1)
    for (int i = 0; i < DEV_CARD_COUNT; i++) {

        // 1. 将整数索引 i 转换回 DevCardType 枚举类型
        DevCardType currentType = (DevCardType)i;

        // 2. 从我们的数组中获取卡牌的名称
        std::string name = devCardNames[i];

        // 3. 使用你已有的函数获取玩家拥有的该卡牌的数量
        int count = player.getDevCardCount(currentType);

        // 4. 使用 std::string 拼接成最终的字符串, e.g., "Knight: 2"
        std::string cardStr = name + ": " + std::to_string(count);

        // 5. 在正确的位置绘制字符串
        // 每一行的 Y 坐标在前一行的基础上向下偏移 30 像素
        outtextxy(COL1_X, DEV_Y + (i * 30), cardStr.c_str());
    }
}

void PlayerPanel::drawStatusMessage(GameState state) {
    setbkmode(TRANSPARENT);
    settextcolor(BLUE);
    settextstyle(22, 0, _T("Arial Black"));

    std::string msg = "";
    switch(state) {
        case GameState::IDLE:
            msg = "Your turn. Roll or take action.";
            break;
        case GameState::AWAITING_MAP_CLICK_FOR_SETTLEMENT:
            msg = "ACTION: Build Settlement\nPlease switch to map and select a location.";
            break;
        case GameState::AWAITING_MAP_CLICK_FOR_ROAD:
            msg = "ACTION: Build Road\nPlease switch to map and select a location.";
            break;
        case GameState::AWAITING_MAP_CLICK_FOR_CITY:
            msg = "ACTION: Build City\nPlease switch to map and select a location.";
            break;
    }
    outtextxy(COL3_X - 50, STATUS_Y, msg.c_str());
    settextcolor(BLACK); // 重置
}

void PlayerPanel::drawButtons(const Player& player, GameState state) {

    // *** 这是你的 UI 逻辑核心 ***

    if (state == GameState::IDLE) {
        // --- 处于 IDLE 状态：启用/禁用标准按钮 ---

        // 1. 根据资源决定是否"灰显"
        buttonMap[ButtonTypeFromPanel::BUILD_ROAD]->enabled = player.canAffordBuilding(ROAD);
        buttonMap[ButtonTypeFromPanel::BUILD_SETTLEMENT]->enabled = player.canAffordBuilding(SETTLEMENT);
        buttonMap[ButtonTypeFromPanel::BUILD_CITY]->enabled = player.canAffordBuilding(CITY);
        buttonMap[ButtonTypeFromPanel::BUY_DEV_CARD]->enabled = player.canAffordDevCard();

        // 2. 根据逻辑决定
        buttonMap[ButtonTypeFromPanel::USE_DEV_CARD]->enabled = true; // 总是可以进入发展卡页面

        buttonMap[ButtonTypeFromPanel::TRADE_BANK]->enabled = true; // 总是可以进入银行页面

        // 3. 流程按钮
        buttonMap[ButtonTypeFromPanel::SWITCH_TO_MAP]->enabled = true;
        buttonMap[ButtonTypeFromPanel::END_TURN]->enabled = true;


    } else {
        // --- 处于 AWAITING (等待地图点击) 状态 ---

        // 1. 禁用所有动作按钮
        buttonMap[ButtonTypeFromPanel::BUILD_ROAD]->enabled = false;
        buttonMap[ButtonTypeFromPanel::BUILD_SETTLEMENT]->enabled = false;
        buttonMap[ButtonTypeFromPanel::BUILD_CITY]->enabled = false;
        buttonMap[ButtonTypeFromPanel::BUY_DEV_CARD]->enabled = false;
        buttonMap[ButtonTypeFromPanel::USE_DEV_CARD]->enabled = false;
        buttonMap[ButtonTypeFromPanel::TRADE_BANK]->enabled = false;
        buttonMap[ButtonTypeFromPanel::END_TURN]->enabled = false;

        // 2. 启用 "Switch to Map" (提醒玩家)
        buttonMap[ButtonTypeFromPanel::SWITCH_TO_MAP]->enabled = true;

    }

    // --- 统一绘制所有按钮 ---
    // (我们只绘制 enabled=false 的 Cancel 按钮，但不让它可见)
    for (auto const& [type, button] : buttonMap) {
        // 特殊处理：IDLE 状态下不绘制 "Cancel" 按钮
        if (state == GameState::IDLE && type == ButtonTypeFromPanel::CANCEL_ACTION) {
            continue;
        }
        button->draw();
    }
}