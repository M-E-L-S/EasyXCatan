#pragma once
#include "Common.h"
#include "Windows.h"

// ----------- Player 类的定义 -----------
class Player{

public: // 外部可以访问的

    // --- 构造函数 ---
    // 创建一个新玩家时调用，必须指定 ID 和颜色
    Player(int id, COLORREF playerColor);

    // ---  getter (获取数据) 函数 ---
    int getVictoryPoints() const; // 动态计算并返回玩家的当前分数

    int getTotalResourceCount() const; // 获取总资源卡数量：用于弃牌
    int getResourceCount(ResourceType type) const; // 获取特定资源的数量
    int getBuildingCount(BuildingType type) const; // 获取特定建筑的数量
    int getDevCardCount(DevCardType type) const; // 获取特定发展卡的数量
    int getKnightCount() const;       // 获取已使用的骑士卡数量
    bool getHasLargestArmy() const;

    COLORREF getColor() const; // 获取玩家的颜色
    int getID() const;         // 获取玩家的 ID (0 或 1)

    // --- 规则检查 (Affordability) 函数 ---
    bool canAffordBuilding(BuildingType type) const;
    bool canAffordDevCard() const;
    bool hasDevCard(DevCardType type) const;
    bool hasHarbor() const;

    // --- 动作 (Action) 函数 ---
    // 这些函数会修改私有数据
    void addResource(ResourceType type, int amount);
    void removeResource(ResourceType type, int amount);
    void addUsedKnight();

    // !!注意：这些函数只修改玩家数据不扣除相应的资源
    void addDevCard(DevCardType type);
    void removeDevCard(DevCardType type);// 使用发展卡时应调用
    void addBuilding(BuildingType type);// 添加城市时不会自动减少村庄
    void removeBuilding(BuildingType type);// 添加城市时需调用以减少村庄
    // 支付（单独的函数来扣资源）
    void payForBuilding(BuildingType type);
    void payForDevCard();

    // 特殊奖项(需要map统计调用该接口修改是否有最x的xx)
    void setHasLongestRoad(bool hasIt);
    void setHasLargestArmy(bool hasIt);

    // 计算需要弃牌的数量（逻辑不变，底层用数组求和）
    int getDiscardCount() const;
    bool discardResources(const int discardArray[RESOURCE_COUNT]);


private: // 只有 Player 类内部能访问

    // --- 核心数据 ---
    int playerID;       // 玩家 ID (0 或 1)
    COLORREF color;     // 玩家的颜色 (例如 RED, BLUE)

    // 资源
    int resourceNumber[RESOURCE_COUNT]; // 存储 5 种资源数量的数组
    int buildingNumber[BUILDING_COUNT]; // 存储 3 种建筑数量的数组

    // 发展卡
    int devCardNumber[DEV_CARD_COUNT];  // 存储 5 种发展卡数量的数组
    int usedKnightNumber;               // 已打出的骑士卡数量
    // 分数相关的特殊状态
    bool hasLongestRoad; // 拥有最长道路 (2分)
    bool hasLargestArmy; // 拥有最大军队 (2分)
    bool hasHarbor_;

};