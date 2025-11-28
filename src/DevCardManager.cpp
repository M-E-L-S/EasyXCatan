#include "DevCardManager.h"
#include <algorithm> // for std::find, remove
#include <stdexcept>

// Helper: 在本 manager->players 中查找对应的局部索引。
// 1) 如果 playerID 在 [0, players.size()) 内，优先直接视为局部索引。
// 2) 否则按 players[i]->getID() 匹配，找到则返回该局部索引。
// 3) 找不到则返回 -1（调用者应当做安全处理，避免越界访问）。
static int findLocalIndex(const std::vector<Player*>& playersRef, int playerID) {
    if (playerID >= 0 && playerID < (int)playersRef.size()) return playerID;
    for (size_t i = 0; i < playersRef.size(); ++i) {
        if (playersRef[i] && playersRef[i]->getID() == playerID) return (int)i;
    }
    return -1;
}

DevCardManager::DevCardManager(std::vector<Player*>& players)
    : players(players),
    justBought(players.size())
{
}

// ================ 基础功能 ================

void DevCardManager::AddCard(int playerID, DevCardType card) {
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) {
        // 无法映射到本 manager 的玩家（安全返回）
        return;
    }
    if (players[idx]) players[idx]->addDevCard(card);

    // 确保 justBought 有对应槽
    if ((int)justBought.size() <= idx) justBought.resize(idx + 1);
    justBought[idx].push_back(card);
}

bool DevCardManager::CanPlayCard(int playerID, DevCardType card)const{
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) return false; // 无法找到玩家 -> 认为不可用，安全返回

    // 本回合买的不能用（检查 justBought 边界）
    if (idx < (int)justBought.size()) {
        for (auto c : justBought[idx]) {
            if (c == card) return false;
        }
    }

    // 检查玩家是否有该卡
    if (!players[idx]) return false;
    return players[idx]->getDevCardCount(card) > 0;
}

bool DevCardManager::PlayCard(int playerID, DevCardType card) {
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) return false;
    if (!CanPlayCard(playerID, card)) return false;

    if (!players[idx]) return false;
    players[idx]->removeDevCard(card);
    return true;
}

void DevCardManager::EndTurn() {
    for (auto& v : justBought)
        v.clear();
}

int DevCardManager::getNewThisTurn(int playerID, DevCardType card) const {
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) return 0;

    if (idx >= (int)justBought.size()) return 0;

    int count = 0;
    for (auto& c : justBought[idx]) {
        if (c == card) count++;
    }
    return count;
}


// ================ 卡牌效果 ================

std::vector<ResourceType> DevCardManager::YearOfPlenty(int playerID, const std::vector<ResourceType>& chosenResources) {
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) return {};
    if (!players[idx]) return {};

    for (auto &r : chosenResources) {
        players[idx]->addResource(r, 1);
    }
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
    // 注意：allPlayers 是调用者传入的“全局玩家指针数组”
    int total = 0;
    for (size_t i = 0; i < allPlayers.size(); i++) {
        Player* p = allPlayers[i];
        if (!p) continue;
        // 不能针对自己执行抽取：比较玩家 ID（更稳健）
        if (p->getID() == playerID) continue;
        int amt = p->getResourceCount(type);
        if (amt > 0) {
            total += amt;
            p->removeResource(type, amt);
        }
    }

    // 把收集到的资源给调用者在本 manager 的 players 中查找并添加
    int idx = findLocalIndex(players, playerID);
    if (idx != -1 && players[idx]) {
        players[idx]->addResource(type, total);
    } else {
        // 找不到本地玩家槽：尝试在 allPlayers 中找到并加（防御）
        for (auto p : allPlayers) {
            if (p && p->getID() == playerID) {
                p->addResource(type, total);
                break;
            }
        }
    }
    return type;
}

bool DevCardManager::AddVictoryPoint(int playerID) {
    int idx = findLocalIndex(players, playerID);
    if (idx == -1) return false;
    if (!players[idx]) return false;
    players[idx]->addDevCard(DevCardType::VICTORY_POINT); // 隐藏 VP
    return true;
}