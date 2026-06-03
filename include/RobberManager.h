// RobberManager.h
#pragma once
#include "Common.h"
#include "Player.h"

class RobberManager {
public:
    // 静态成员函数声明（无需实例化，直接类名调用）
    static int stealResource(Player& robber, Player& victim);
};