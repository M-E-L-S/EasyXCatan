#include "DevCardManager.h"

DevCardManager::DevCardManager(std::vector<Player*>& players)
    : players(players),
    justBought(players.size())
{
}

// ================ 基础功能 ================

void DevCardManager::AddCard(int playerID, DevCardType card) {
    players[playerID]->addDevCard(card);
    justBought[playerID].push_back(card);
}

bool DevCardManager::CanPlayCard(int playerID, DevCardType card)const{
    // 本回合买的不能用
    for (auto c : justBought[playerID])
        if (c == card) return false;

    return players[playerID]->getDevCardCount(card) > 0;
}

bool DevCardManager::PlayCard(int playerID, DevCardType card) {
    if (!CanPlayCard(playerID, card)) return false;

    players[playerID]->removeDevCard(card);
    return true;
}

void DevCardManager::EndTurn() {
    for (auto& v : justBought)
        v.clear();
}

int DevCardManager::getNewThisTurn(int playerID, DevCardType card) const {
    if (playerID < 0 || playerID >= justBought.size()) return 0;

    int count = 0;
    for (auto& c : justBought[playerID]) {
        if (c == card) count++;
    }
    return count;
}


// ================ 卡牌效果 ================

std::vector<ResourceType> DevCardManager::YearOfPlenty(int playerID, const std::vector<ResourceType>& chosenResources) {
    return chosenResources; // UI 层传入玩家选择的两张资源
}

int DevCardManager::RoadBuilding(int playerID) {
    return 2; // 固定放两条路
}

bool DevCardManager::UseKnight(Player& player, std::vector<Player>& allPlayers) {
    player.addUsedKnight(); // Player 内部维护 usedKnightNumber

    // 检查是否触发最大骑士数变化
    int maxKnights = 0;
    int ownerID = -1;
    for (auto& p : allPlayers) {
        if (p.getKnightCount() > maxKnights) {
            maxKnights = p.getKnightCount();
            ownerID = p.getID();
        }
    }

    // 如果有新的最大骑士数且 >= 3，更新玩家状态
    if (maxKnights >= 3) {
        for (auto& p : allPlayers) {
            p.setHasLargestArmy(p.getID() == ownerID);
        }
        return true;
    }
    return false;
}


ResourceType DevCardManager::Monopoly(int playerID, ResourceType type,
    std::vector<Player*>& allPlayers)
{
    int total = 0;
    for (size_t i = 0; i < allPlayers.size(); i++) {
        if ((int)i == playerID) continue;
        int amt = allPlayers[i]->getResourceCount(type);
        total += amt;
        allPlayers[i]->removeResource(type, amt);
    }
    players[playerID]->addResource(type, total);
    return type;
}

bool DevCardManager::AddVictoryPoint(int playerID) {
    players[playerID]->addDevCard(DevCardType::VICTORY_POINT); // 隐藏 VP
    return true;
}