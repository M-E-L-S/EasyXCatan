// PlayerPanel.h
// 该部分没有循环能力，只能被调用画图
#pragma once
#include <vector>
#include <map>
#include <string>
#include "Button.h"
#include "Player.h"       // 需要 Player 来显示数据
#include "Common.h"       // 需要 Common 来获取资源枚举

class PlayerPanel {
public:

    // 构造函数
    PlayerPanel(int screenWidth, int screenHeight);

    /*
     * @brief 绘制整个全屏面板 (GameController 每一帧都会调用)
     * @param player 当前玩家 (用于显示资源、决定按钮是否灰显)
     * @param state  当前游戏状态 (用于显示 "请在地图上选点")
     */
    void draw(const Player& player, GameState state);

    /*
     * @brief 检查鼠标点击 (GameController 在收到点击时调用)
     * @return 被点击的按钮类型 (ButtonType)
     */
    ButtonTypeFromPanel getClickedButton(int mouseX, int mouseY);

private:
    // --- 绘制辅助函数 ---
    void drawResources(const Player& player);
    void drawDevCards(const Player& player);
    void drawStatusMessage(GameState state);
    void drawButtons(const Player& player, GameState state);
    void drawPlayerInfo(const Player& player);

    // --- 屏幕尺寸 ---
    int width;
    int height;

    // --- UI 控件 ---
    // 我们把所有按钮都存在一个 vector 里
    std::vector<Button> buttons;

    // 我们用一个 map 来快速查找按钮，这比遍历 vector 要高效
    std::map<ButtonTypeFromPanel, Button*> buttonMap;
};
