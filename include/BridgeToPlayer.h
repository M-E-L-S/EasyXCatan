#pragma once
#include"Player.h"
#include<graphics.h>
#include "RobberManager.h"
#include "RobbedResourcePanel.h"
#include "DiscardPanel.h"
#include "Common.h"
#include <vector>

using namespace std;

inline COLORREF playerColor[5]={
    RGB(231,103,39),
    RGB(231,103,39),
    RGB(38,111,64),
    RGB(22,81,136),
    RGB(63,58,57)
};

vector<Player> players;

inline void PlayerInit(int PlayerId) {
    Player player(PlayerId, playerColor[PlayerId]);
    players.push_back(player);
}

inline void Resources_Add(int playerId, vector<pair<ResourceType, int>> number) {
    for (auto pair : number)
        players[playerId].addResource(pair.first,pair.second);
}

inline int Score_CheckVictory(int playerId, int LongestRoadScore) {
    return players[playerId].getVictoryPoints() + LongestRoadScore;
}

inline Player& getPlayer(int playerId) {
    return players[playerId];
}

int Users_CheckMostKnight() {
    int max_id=0,max=players[0].getKnightCount();
    for (int i=1;i<players.size();i++) {
        int new_=players[i].getKnightCount();
        if (new_>max_id) {max_id=new_;max=new_;}
    }
    return max_id;
}

void Resources_Discard() {
    for (int i = 0; i < 4; i++) {
        if(getPlayer(i).getDiscardCount()>0){
            const char* resPaths[RESOURCE_COUNT] = {
                "./assets/wood.jpg", "./assets/brick.jpg",
                "./assets/sheep.jpg", "./assets/wheat.jpg", "./assets/ore.jpg"
            };
            DiscardPanel panel(SCREEN_WIDTH, SCREEN_HEIGHT, getPlayer(i));
            panel.loadResourceImages(resPaths);
            panel.loadFullScreenBackground("./assets/background.jpg");

            panel.show(); // 显示弃牌面板
        }
    }
}
// 补全后的 Resources_Discard（玩家抽资源，完美对接 RobbedResourcePanel）
void Resources_Discard(int playerId, int victim) {
    // 1. 获取玩家实例（你已有 getplayer 函数，无需修改）
    Player& currentPlayer = getPlayer(playerId-1);
    Player& victimPlayer = getPlayer(victim-1);

    // 2. 调用偷资源函数：获取抢夺结果（返回值 = -1=失败，0-4=资源序号，对应面板的 stolenResType）
    int stolenResIdx = RobberManager::stealResource(currentPlayer, victimPlayer);

    // 3. 配置资源图片路径（必须和面板的 RESOURCE_COUNT=5 对应，顺序 0-4）
    // 注意：路径格式为 BMP，用绝对路径或正确相对路径，避免加载失败
    const char* resImagePaths[RESOURCE_COUNT] = {
        "./assets/wood.jpg", "./assets/brick.jpg",
        "./assets/sheep.jpg", "./assets/wheat.jpg", "./assets/ore.jpg"
    };

    // 4. 对接 RobbedResourcePanel 接口，显示结果（分成功/失败，面板自动处理）
    RobbedResourcePanel resultPanel(
        SCREEN_WIDTH,  // 窗口宽度（来自 Common.h，面板构造函数要求）
        SCREEN_HEIGHT, // 窗口高度（来自 Common.h）
        stolenResIdx,  // 抢夺结果（-1=扑空，0-4=资源序号，面板直接识别）
        victim         // 被抢夺者ID（面板显示 "P+victim"）
    );

    // 5. 加载资源图片（可选，无图时面板自动显示对应色块）
    resultPanel.loadResourceImages(resImagePaths);
    resultPanel.loadFullScreenBackground("./assets/background.jpg");

    // 6. 显示面板（阻塞式，点击"确 定"关闭，面板内部已实现交互）
    resultPanel.show();
} //玩家抽资源