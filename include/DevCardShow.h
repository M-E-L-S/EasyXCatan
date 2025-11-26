#pragma once
#include "DevCardPanel.h"
#include "Player.h"
#include "DevCardManager.h"

// 显示发展卡面板并返回玩家选择的卡牌类型
// 返回值：0~DEV_CARD_COUNT-1 对应卡牌类型，-1表示取消或关闭
int ShowDevCardPanel(Player& player, DevCardManager& manager, DevCardPanel& panel);