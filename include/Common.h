//该头文件定义基本的游戏状态、资源建筑发展卡的类型与成本与分数

#pragma once

// 状态 1: 当前显示哪个全屏 (GameController 拥有)
enum class ActiveScreen {
    VIEW_MAP,       // 显示全屏地图
    VIEW_PLAYER_PANEL,  // 显示全屏玩家面板
    VIEW_BANK_PANEL,
    VIEW_DEV_CARD_PANEL,
};

// 状态 2: 当前正在执行什么动作 (GameController 拥有)
enum class GameState {
    IDLE,             // 默认状态，等待玩家操作
    AWAITING_MAP_CLICK_FOR_ROAD,
    AWAITING_MAP_CLICK_FOR_SETTLEMENT,
    AWAITING_MAP_CLICK_FOR_CITY,
    AWAITING_MAP_CLICK_FOR_THIEF,
    AWAITING_MAP_CLICK_FOR_KNIGHT,
    // ... 其他状态
};

// GameController 会询问这个枚举
// 来知道是哪个按钮被点击了
enum class ButtonTypeFromPanel {
    NONE,
    BUILD_ROAD,
    BUILD_SETTLEMENT,
    BUILD_CITY,
    BUY_DEV_CARD,
    USE_DEV_CARD,
    TRADE_BANK,
    DISCARD_CARD,
    ROLL_DICE,
    END_TURN,
    SWITCH_TO_MAP,
    //用于从银行，发展卡等界面返回个人界面
    CANCEL
};

// 定义资源类型
// 用 resources[WOOD] 来访问木
enum ResourceType
{
    WOOD,   // 木头
    BRICK,  // 砖块
    SHEEP,  // 羊
    WHEAT,  // 小麦
    ORE,    // 石头
    RESOURCE_COUNT // 资源种类的总数 (5)
};

// 地块资源类型
enum TerrainType {
    FOREST,    // 森林 -> 木头
    HILLS,     // 丘陵 -> 砖
    PASTURE,   // 牧场 -> 羊
    FIELDS,    // 田地 -> 麦子
    MOUNTAINS, // 山脉 -> 矿石
    DESERT,    // 沙漠 -> 无资源
    TERRAIN_COUNT
};

// 定义建筑类型
enum BuildingType
{
    ROAD,   // 路
    SETTLEMENT,  // 村庄
    CITY,  // 城市
    BUILDING_COUNT // 建筑种类的总数 (3)
};

// 定义发展卡类型
enum DevCardType
{
    KNIGHT,         // 骑士
    VICTORY_POINT,  // 分数
    ROAD_BUILDING,  // 修路
    MONOPOLY,       // 垄断
    YEAR_OF_PLENTY, // 丰收
    DEV_CARD_COUNT
};

// 建筑成本表
const int BUILDING_COSTS[BUILDING_COUNT][RESOURCE_COUNT] = {
    // WOOD, BRICK, SHEEP, WHEAT, ORE
    {1, 1, 0, 0, 0},  // ROAD
    {1, 1, 1, 1, 0},  // SETTLEMENT
    {0, 0, 0, 2, 3}   // CITY
};

// 建筑得分表
const int BUILDING_POINTS[BUILDING_COUNT]= {
    0,1,2
};

