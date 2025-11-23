#pragma once
#include <vector>
#include <string>

using namespace std;

/* -----------------------
基本类型与枚举
    ----------------------- */
enum class ResourceType { Wood, Brick, Wheat, Sheep, Ore, ALL, None };
enum class BuildType { Road, Settlement, City, DevCard, None };
enum class TradeType { Bank, Player, Port, None };
enum class DevCardType { Knight, VictoryPoint, Monopoly, RoadBuilding, YearOfPlenty, None };


/* -----------------------
外部接口
    ----------------------- */

// ----- 地图组 -----
// 初始化地图
extern void Map_Init(int seed);
// 地图绘制
extern void Map_Draw();
// 玩家交易属性（0元购, 4:1, 3:1, 2:1……）
extern vector<bool> Map_GetTradeOption(int playerId);
// 资源产出(ID, type, amount)
extern vector<vector<pair<ResourceType, int>>> Map_ProduceResources(int diceRoll);
extern vector<pair<ResourceType, int>> Map_PreResources(int playerId);
// 玩家建设
extern void Map_HandleBuildRequest(BuildType type, int playerId, bool isPre = false);
extern bool Map_HandleBuildRequest(BuildType type, int playerId, int mouseX, int mouseY, bool isPre = false);
// 长路特判(ID)
extern int Map_CheckLongestRoad();
// 强盗移动(ID List)
extern vector<int> Map_MoveRobber(int mouseX, int mouseY);

// ----- 用户组 -----
// 玩家初始化
extern void Users_RegisterPlayer(const string &name, int id);
extern void Users_Draw(int playerId);
// 资源管理
extern void Resources_Dec(int playerId, BuildType type, int amount);
extern void Resources_Add(int playerId, vector<pair<ResourceType, int>>);
// 打出发展卡
extern DevCardType Users_PlayDevCardUI(int playerId);
// 分数管理(胜利判定)
extern bool Score_CheckVictory(int playerId, int LongestRoadScore);
// 资源卡数量检测（amount）
extern int Resources_CheckDiscardCount(int playerId, ResourceType res);
// 丢弃资源
extern void Resources_DoDiscard(int playerId);
extern ResourceType Resources_DoDiscard(int playerId, int mouseX, int mouseY);
// 抢劫
extern ResourceType Resources_HandleRobbery(int mouseX, int mouseY);

// ----- 交易组 -----
extern void Trade_Draw();
// 玩家购买
extern bool Trade_PlayerBuy(int playerId, int mouseX, int mouseY);
// 银行交易
extern void Trade_BankTrade(int playerId, const vector<bool> &give);
// 玩家间交易
extern void Trade_PlayerTradeUI(int offeringPlayer, int receivingPlayer);

#ifndef EASYX_PROJECT_MAIN_H
#define EASYX_PROJECT_MAIN_H

#endif //EASYX_PROJECT_MAIN_H