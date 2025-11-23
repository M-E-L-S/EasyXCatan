#pragma once
#include <graphics.h>
#include "Player.h"
#include "PlayerPanel.h"
#include "Common.h"
#include "TextWindow.h"
#include "Random.h"

// 声明你已经写好的外部循环函数
// 必须传入非 const 引用，因为交易和买卡会修改玩家数据
extern void bankloop(Player& player);
extern void devcardloop(Player& player);


// 转换函数：将 DevCardType 转换为字符串
std::string DevCardTypeToString(DevCardType card) {
    switch (card) {
    case KNIGHT:
        return "骑士 (Knight)";
    case VICTORY_POINT:
        return "胜利点 (Victory Point)";
    case ROAD_BUILDING:
        return "修路 (Road Building)";
    case MONOPOLY:
        return "垄断 (Monopoly)";
    case YEAR_OF_PLENTY:
        return "丰饶年 (Year of Plenty)";
    default:
        return "未知开发卡 (Unknown)";
    }
}
/*
 * 玩家操作界面循环
 * @param player: 当前玩家（引用，因为可能修改资源）
 * @param state: 当前游戏状态（引用，用于切换到“建设中”状态）
 * @param lastDiceResult: 上次掷骰子点数，用于显示
 */
void PlayerLoop(Player& player, GameState& state, int lastDiceResult) {

    // 1. 初始化面板
    // 注意：如果游戏频繁进出此循环，建议将 panel 作为参数传入 PlayerLoop，
    // 否则每次进入都会重新加载图片，影响性能。
    PlayerPanel panel(SCREEN_WIDTH, SCREEN_HEIGHT);

    // 加载资源图片 (路径请根据你的实际项目结构调整)
    // 如果你在 main 中已经加载过，这里可以省略，或者最好将 panel 传进来
    const char* resPaths[RESOURCE_COUNT] = {
        "./assets/wood.png",
        "./assets/brick.png",
        "./assets/sheep.png",
        "./assets/wheat.png",
        "./assets/ore.png"
    };
    panel.loadResourceImages(resPaths); // 如果需要加载图片请取消注释

    // 开启批量绘图，防闪烁
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
        // 使用 EasyX 的非阻塞消息处理
        ExMessage msg;

        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {

                // 获取点击了哪个按钮
                btn = panel.getClickedButton(msg.x, msg.y);

                // --- 逻辑处理部分 ---
                switch (btn) {
                    // 1. 交易与银行
                case ButtonTypeFromPanel::TRADE_BANK:
                    // 暂时结束当前批量绘图，以免子循环冲突
                    EndBatchDraw();
                    textWindow("Here is bank.");
                    //bankloop(player);
                    // 子循环结束后重新开启
                    BeginBatchDraw();
                    break;

                    // 2. 发展卡相关
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
                    textWindow("Here is devCardPanel.");
                    //devcardloop(player);
                    BeginBatchDraw();
                    break;

                    // 3. 切换回地图 / 取消 / 结束回合
                case ButtonTypeFromPanel::SWITCH_TO_MAP:
                    // 直接退出循环，返回主循环
                    state = GameState::AWAITING_SWITCH_TO_MAP;
                    exitLoop = true;
                    break;

                case ButtonTypeFromPanel::END_TURN:
                    // 你可能需要一种机制告诉主循环“回合结束”
                    // 这里假设主循环会检查某种标志，或者仅仅是退出面板
                    state = GameState::END_TURN;
                    exitLoop = true;
                    break;

                    // 4. 建设指令 (需要切回地图选点)
                case ButtonTypeFromPanel::BUILD_ROAD:
                    if (player.canAffordBuilding(ROAD)) {
                        player.payForBuilding(ROAD);
                        player.addBuilding(ROAD);
                        state = GameState::AWAITING_MAP_CLICK_FOR_ROAD;
                        exitLoop = true;
                    }
                    else {
                        // 调用 textWindow 函数显示提示，会阻塞直到用户点击确定
                        EndBatchDraw(); // 结束 PlayerLoop 的批量绘图
                        textWindow("Can't afford build road!");
                        BeginBatchDraw(); // 警告框结束后重新开启批量绘图
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

        // 简单的帧率控制，避免 CPU 占用过高
        Sleep(16);
    }

    EndBatchDraw();
}