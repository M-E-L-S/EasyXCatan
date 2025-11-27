// RobberManager.cpp
#include "RobberManager.h"
#include <vector>
#include <random>

// 静态函数实现：强盗偷资源
int RobberManager::stealResource(Player& robber, Player& victim) {
    // 收集受害者有资源的数组索引（0-4）
    std::vector<int> availableResIndices;
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (victim.getResourceCount((ResourceType)i) > 0) {
            availableResIndices.push_back(i);
        }
    }

    // 无可用资源，返回 -1（失败）
    if (availableResIndices.empty()) {
        return -1;
    }

    // 随机选择一种资源（拿到资源序号 0-4）
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, availableResIndices.size() - 1);
    int selectedIdx = availableResIndices[dist(gen)]; // 0-4 的资源序号

    // 转移资源
    victim.removeResource((ResourceType)selectedIdx, 1);
    robber.addResource((ResourceType)selectedIdx, 1);

    // 返回抢到的资源序号（0-4）
    return selectedIdx;
}