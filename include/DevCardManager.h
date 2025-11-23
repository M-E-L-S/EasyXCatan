#pragma once
#include <vector>
#include "Common.h"
#include "Player.h"

class DevCardManager {
public:
    DevCardManager(std::vector<Player*>& players);

    // ================ 基础功能 ================
    void AddCard(int playerID, DevCardType card);            // 给玩家发一张卡（购买）
    bool CanPlayCard(int playerID, DevCardType card)const;       // 判断本回合买是否能用
    bool PlayCard(int playerID, DevCardType card);          // 使用卡（成功返回 true）
    void EndTurn();                                         // 回合结束，清空回合彩买限制
    int getNewThisTurn(int playerID, DevCardType card) const;  //获取这回合获得的发展卡数量

    // ================ 卡牌效果 ================
    std::vector<ResourceType> YearOfPlenty(int playerID, const std::vector<ResourceType>& chosenResources); // 丰收之年
    int RoadBuilding(int playerID);                        // 返回玩家能放的道路数（固定2）
    bool UseKnight(Player& player, std::vector<Player>& allPlayers);                           // 返回是否触发“最大骑士数变化”
    ResourceType Monopoly(int playerID, ResourceType type,
        std::vector<Player*>& allPlayers); // 垄断
    bool AddVictoryPoint(int playerID);                     // +1 VP（隐藏）

private:
    std::vector<Player*>& players;                          // 所有玩家对象引用
    std::vector<std::vector<DevCardType>> justBought;       // 本回合买的卡，不能用
};
#pragma once
