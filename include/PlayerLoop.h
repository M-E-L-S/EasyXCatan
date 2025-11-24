#pragma once
#include <graphics.h>
#include "Player.h"
#include "PlayerPanel.h"
#include "Common.h"
#include "TextWindow.h"
#include "Random.h"
#include "DevCardLoop.h"
#include "BankLoop.h"


// 转换函数：将 DevCardType 转换为字符串
std::string DevCardTypeToString(DevCardType card) {
    switch (card) {
    case KNIGHT:
        return "(Knight)";
    case VICTORY_POINT:
        return "(Victory Point)";
    case ROAD_BUILDING:
        return "(Road Building)";
    case MONOPOLY:
        return "(Monopoly)";
    case YEAR_OF_PLENTY:
        return "(Year of Plenty)";
    default:
        return "(Unknown)";
    }
}
/*
 * 玩家操作界面循环
 * @param player: 当前玩家（引用，因为可能修改资源）
 * @param state: 当前游戏状态（引用，用于切换到“建设中”状态）
 * @param lastDiceResult: 上次掷骰子点数，用于显示
 */
void PlayerLoop(Player& player, GameState& state, int lastDiceResult) {

    //  初始化面板
    PlayerPanel panel(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    // 加载资源图片 (路径请根据你的实际项目结构调整)
    const char* resPaths[RESOURCE_COUNT] = {
        "./assets/wood.jpg",
        "./assets/brick.jpg",
        "./assets/sheep.jpg",
        "./assets/wheat.jpg",
        "./assets/ore.jpg"
    };
    panel.loadResourceImages(resPaths);
    panel.loadBackgroundImage("assets/player_bg.jpg");


    BeginBatchDraw();

    bool exitLoop = false;
    ButtonTypeFromPanel btn;

    while (!exitLoop) {
        // --- 渲染部分 ---
        cleardevice();

        // 调用 PlayerPanel 的绘制函数
        panel.draw(player, state, lastDiceResult);

        FlushBatchDraw();

        // --- 输入检测部分 ---
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {

                // 获取点击了哪个按钮
                btn = panel.getClickedButton(msg.x, msg.y);

                // --- 逻辑处理部分 ---
                switch (btn) {
                    // 交易与银行
                case ButtonTypeFromPanel::TRADE_BANK:
                    // 暂时结束当前批量绘图，以免子循环冲突
                    EndBatchDraw();
                    EnterBankMode(player);
                    BeginBatchDraw();
                    break;

                    // 发展卡相关
                case ButtonTypeFromPanel::BUY_DEV_CARD:
                    EndBatchDraw();
                    if (!player.canAffordDevCard())
                        textWindow("Can't afford dev card");
                    else {
                        DevCardType devCard = (DevCardType)random_number(0, DEV_CARD_COUNT - 1);
                        textWindow("Bought Dev Card:" + DevCardTypeToString(devCard));
                        player.payForDevCard();
                        player.addDevCard(devCard);
                    }
                    BeginBatchDraw();
                    break;
                case ButtonTypeFromPanel::USE_DEV_CARD:
                    EndBatchDraw();
                    DevCardLoop(player);
                    BeginBatchDraw();
                    break;

                    // 切换回地图 / 取消 / 结束回合
                case ButtonTypeFromPanel::SWITCH_TO_MAP:
                    // 直接退出循环，返回主循环
                    state = GameState::AWAITING_SWITCH_TO_MAP;
                    exitLoop = true;
                    break;

                case ButtonTypeFromPanel::END_TURN:
                    state = GameState::END_TURN;
                    exitLoop = true;
                    break;

                    //  建设指令 (需要切回地图选点)
                case ButtonTypeFromPanel::BUILD_ROAD:
                    if (player.canAffordBuilding(ROAD)) {
                        player.payForBuilding(ROAD);
                        player.addBuilding(ROAD);
                        state = GameState::AWAITING_MAP_CLICK_FOR_ROAD;
                        exitLoop = true;
                    }
                    else {
                        EndBatchDraw();
                        textWindow("Can't afford build road!");
                        BeginBatchDraw();
                    }
                    break;

                case ButtonTypeFromPanel::BUILD_SETTLEMENT:
                    if (player.canAffordBuilding(SETTLEMENT)) {
                        player.payForBuilding(SETTLEMENT);
                        player.addBuilding(SETTLEMENT);
                        state = GameState::AWAITING_MAP_CLICK_FOR_SETTLEMENT;
                        exitLoop = true;
                    }
                    else {
                        EndBatchDraw();
                        textWindow("Can't afford build settlement!");
                        BeginBatchDraw();
                    }
                    break;

                case ButtonTypeFromPanel::BUILD_CITY:
                    if (player.canAffordBuilding(CITY)) {
                        player.payForBuilding(CITY);
                        player.addBuilding(CITY);
                        player.removeBuilding(SETTLEMENT);
                        state = GameState::AWAITING_MAP_CLICK_FOR_CITY;
                        exitLoop = true;
                    }
                    else {
                        EndBatchDraw();
                        textWindow("Can't afford build city!");
                        BeginBatchDraw();
                    }
                    break;

                default:
                    break;
                }
            }
        }

        Sleep(16);
    }

    EndBatchDraw();
}