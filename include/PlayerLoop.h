//PlayerLoop.h
#pragma once
#include <graphics.h>
#include "Player.h"
#include "PlayerPanel.h"
#include "Common.h"
#include "TextWindow.h"
#include "imageWindow.h"
#include "Random.h"
#include "DevCardLoop.h"
#include "BankLoop.h"
#include "HarborLoop.h"
#include "BGMusic.h"

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
enum ActionType PlayerLoop(Player& player, int lastDiceResult, std::vector<bool>) {

    //  初始化面板
    GameState state = GameState:: IDLE;
    PlayerPanel panel(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    //MusicManager Music;
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
    MusicManager Music;

    BeginBatchDraw();

    bool exitLoop = false;
    ButtonTypeFromPanel btn;
    //Music.play(MusicType::PANEL);

    while (!exitLoop) {
        // --- 渲染部分 ---
        cleardevice();

        // 调用 PlayerPanel 的绘制函数

        panel.draw(player, state, lastDiceResult);

        FlushBatchDraw();

        // --- 输入检测部分 ---
        ExMessage msg;
        ActionType actiontype;
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
                    Music.play(MusicType::BANK);
                    EnterBankMode(player);
                    Music.play(MusicType::PANEL);
                    BeginBatchDraw();
                    break;

                case ButtonTypeFromPanel::TRADE_HARBOR:
                    // 暂时结束当前批量绘图，以免子循环冲突
                    EndBatchDraw();
                    Music.play(MusicType::HARBOR);
                    EnterHarborMode(player);
                    Music.play(MusicType::PANEL);
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
                    Music.play(MusicType::DEV_CARD);
                    actiontype=DevCardLoop(player);
                    if(actiontype==ActionType::Knight||actiontype==ActionType::RoadBuilding)
                        return actiontype;
                    Music.play(MusicType::PANEL);
                    BeginBatchDraw();
                    break;

                    // 切换回地图 / 取消 / 结束回合
                case ButtonTypeFromPanel::SWITCH_TO_MAP:
                    // 直接退出循环，返回主循环
                    state = GameState::AWAITING_SWITCH_TO_MAP;
                    EndBatchDraw();
					return ActionType::SwitchToMap;
                    exitLoop = true;
                    break;

                case ButtonTypeFromPanel::END_TURN:
                    EndBatchDraw();
                    if (lastDiceResult == 9) {

                        for (int i=0;i<RESOURCE_COUNT;i++)
                            player.addResource(ResourceType(i),1 );

                        textWindow("You got a King Crab, every resource ++!");

                        showImageWindow("./assets/kingcrab.jpg");

                    }
                    if (lastDiceResult == 2) {

                        for (int i=0;i<BUILDING_COUNT;i++)
                            player.removeBuilding(BuildingType(i));

                        textWindow("Anonymous experts took your VP away!");

                        showImageWindow("./assets/anonymous.png");

                    }
                    if (lastDiceResult == 8) {
                        Music.play(MusicType::MANBA);
                        showImageWindow("./assets/manba.png");
                        Music.play(MusicType::PANEL);

                    }

                    state = GameState::END_TURN;

                    return ActionType::EndTurn;;
                    exitLoop = true;
                    break;

                    //  建设指令 (需要切回地图选点)
                case ButtonTypeFromPanel::BUILD_ROAD:
                    if (player.canAffordBuilding(ROAD)) {
                        player.payForBuilding(ROAD);
                        player.addBuilding(ROAD);
                        state = GameState::AWAITING_MAP_CLICK_FOR_ROAD;
                        exitLoop = true;
                        EndBatchDraw();
                        return ActionType::BuildRoad;
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
                        EndBatchDraw();
                        return ActionType::BuildSettlement;;
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
                        EndBatchDraw();
                        return ActionType::BuildCity;;
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
	Music.stop();
    EndBatchDraw();
}