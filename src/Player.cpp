#include "Player.h"

// --- 构造函数 ---
Player::Player(int id, COLORREF playerColor) :
    // 1. C++ 成员初始化列表
    //    在这里初始化简单的变量
    playerID(id),
    color(playerColor),


    // 状态
    usedKnightNumber(0),
    hasLongestRoad(false),
    hasLargestArmy(false)
{
    // 2. 构造函数体
    //    在这里初始化数组，或者执行更复杂的逻辑

    // 将所有资源数量初始化为 0
    for (int i = 0; i < RESOURCE_COUNT; i++)
    {
        resourceNumber[i] = 0;
    }

    // 将所有建筑数量初始化为 0
    for (int i = 0; i < BUILDING_COUNT; i++)
    {
        buildingNumber[i] = 0;
    }

    // 将所有发展卡数量初始化为 0
    for (int i = 0; i < DEV_CARD_COUNT; i++)
    {
        devCardNumber[i] = 0;
    }

    // 在游戏开始时，通常会给玩家一些初始资源
    // if (playerID == 0) {
    //     addResource(WOOD, 1);
    //     addResource(BRICK, 1);
    // }
}

// ---  getter (获取数据) 函数 ---
int Player::getVictoryPoints() const
{
    int totalPoints = 0;

    // 计算建筑的分数
    for (int i = 0; i < BUILDING_COUNT; i++)
        totalPoints += (buildingNumber[i] * BUILDING_POINTS[i]);

    // 計算來自特殊獎項的分數
    if (hasLongestRoad)
        totalPoints += 2;
    if (hasLargestArmy)
        totalPoints += 2;

    // 計算來自發展卡（分數卡）的分數
    totalPoints += devCardNumber[VICTORY_POINT];

    return totalPoints;
}
int Player::getTotalResourceCount() const
{
    int total = 0;
    for (int i = 0; i < RESOURCE_COUNT; i++)
        total += resourceNumber[i];

    return total;
}
int Player::getResourceCount(ResourceType type) const {
    if (type >= 0 && type < RESOURCE_COUNT)
        return resourceNumber[type];
    return 0;
}
int Player::getBuildingCount(BuildingType type) const {
    if (type >= 0 && type < BUILDING_COUNT)
        return buildingNumber[type];
    return 0;
}
int Player::getDevCardCount(DevCardType type) const {
    if (type >= 0 && type < DEV_CARD_COUNT)
        return devCardNumber[type];
    return 0;
}
int Player::getKnightCount() const
{
    return usedKnightNumber;
}

COLORREF Player::getColor() const {
     return color;
}
int Player::getID() const {
    return playerID;
}


// --- 规则检查 (Affordability) 函数 ---
bool Player::canAffordBuilding(BuildingType type) const
{
    switch (type)
    {
        case ROAD:
            // 路：1 木 + 1 磚
            return (resourceNumber[WOOD] >= 1 && resourceNumber[BRICK] >= 1);

        case SETTLEMENT:
            // 村莊：1 木 + 1 磚 + 1 羊 + 1 麥
            return (resourceNumber[WOOD] >= 1 && resourceNumber[BRICK] >= 1 &&
                    resourceNumber[SHEEP] >= 1 && resourceNumber[WHEAT] >= 1);

        case CITY:
            // !! 關鍵邏輯 !!
            // 城市：3 礦 + 2 麥
            // 需要地图端确认选择的地方是否有村庄
            return (resourceNumber[ORE] >= 3 && resourceNumber[WHEAT] >= 2 &&
                    buildingNumber[SETTLEMENT] > 0);
    }

    // 如果傳入無效的類型
    return false;
}
bool Player::canAffordDevCard() const
{
    // 發展卡：1 礦 + 1 羊 + 1 麥
    return (resourceNumber[ORE] >= 1 && resourceNumber[SHEEP] >= 1 && resourceNumber[WHEAT] >= 1);
}
bool Player::hasDevCard(DevCardType type) const {
    if (type >= 0 && type < DEV_CARD_COUNT)
        return devCardNumber[type]!=0;
    return false;
}


// --- 动作 (Action) 函数 ---
void Player::addResource(ResourceType type, int amount)
{
    if (type >= 0 && type < RESOURCE_COUNT && amount > 0)
        resourceNumber[type] += amount;
}
void Player::removeResource(ResourceType type, int amount)
{
    if (type >= 0 && type < RESOURCE_COUNT && amount > 0)
    {
        if (resourceNumber[type] >= amount)
            resourceNumber[type] -= amount;
        else
            // 在一個真實的遊戲中，我們不應該讓這種情況發生
            // 但作為安全防護，我們最多只把它減到 0
            resourceNumber[type] = 0;
    }
}


void Player::addDevCard(DevCardType type) {
    devCardNumber[type]++;
}
void Player::removeDevCard(DevCardType type) {
    if (type >= 0 && type < DEV_CARD_COUNT)
        if (devCardNumber[type] > 0)
            devCardNumber[type]--;
}


void Player::addBuilding(BuildingType type)
{
    buildingNumber[type]++;
}
void Player::removeBuilding(BuildingType type)
{
    buildingNumber[type]--;
}


void Player::payForDevCard()
{
    removeResource(ORE, 1);
    removeResource(SHEEP, 1);
    removeResource(WHEAT, 1);
}
void Player::payForBuilding(BuildingType type)
{
    if (type < 0 || type >= BUILDING_COUNT) return;
    // 遍历 5 种资源
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        // 获取该建筑对该资源的消耗量
        int cost = BUILDING_COSTS[type][i];
        if (cost > 0)
            removeResource((ResourceType)i, cost);
    }
}


void Player::setHasLargestArmy(bool hasIt)
{
    this->hasLargestArmy = hasIt;
}
void Player::setHasLongestRoad(bool hasIt)
{
    this->hasLongestRoad = hasIt;
}