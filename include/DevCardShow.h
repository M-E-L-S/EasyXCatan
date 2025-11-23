#pragma once
#include "DevCardPanel.h"
#include "Player.h"
#include "DevCardManager.h"

// 返回玩家选择的卡牌类型，如果取消或关闭返回 -1
int ShowDevCardPanel(Player& player, DevCardManager& manager);