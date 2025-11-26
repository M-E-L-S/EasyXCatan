#pragma once
#include"Player.h"
#include<graphics.h>
#include <vector>

inline int id=0;
inline COLORREF playerColor[4]={
    RGB(231,103,39),
    RGB(38,111,64),
    RGB(22,81,136),
    RGB(63,58,57)
};

vector<Player> players;

inline void PlayerInit(int PlayerId) {
    Player player(id, playerColor[PlayerId]);
    id++;
    players.push_back(player);
}

inline void Resource_Add(int playerId, vector<pair<ResourceType, int>> number) {
    for (auto pair : number)
        players[playerId].addResource(pair.first,pair.second);
}

inline bool Score_CheckVictory(int playerId, int LongestRoadScore) {
    if (players[playerId].getVictoryPoints()+LongestRoadScore>=10 )
        return true;
    return false;
}

inline Player& getPlayer(int playerId) {
    return players[playerId];
}

int Users_CheckMostKnight() {
    return hasLongestRoad;
}