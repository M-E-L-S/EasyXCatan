#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <ctime>
#include <cassert>

using namespace std;

/* -----------------------
    基本类型与枚举
    ----------------------- */
enum class ResourceType { Wood, Brick, Wheat, Sheep, Ore, ALL, None };
enum class BuildType { Road, Settlement, City, DevCard, DoubleRoad, None };
enum class TradeType { Bank, Player, Port, None };
enum class DevCardType { Knight, VictoryPoint, Monopoly, RoadBuilding, YearOfPlenty, None };

struct MouseEvent {
    bool hasEvent = false;
    int x = 0, y = 0;
    bool leftDown =  false;
    bool rightDown =  false;
};

/* -----------------------
    游戏阶段
    ----------------------- */
enum class TurnPhase {
    PreGameSetup_FirstPlacement,
    PreGameSetup_SecondPlacement,
    DiceRoll,
    ResourceDistribution,
    ResourceThrow,
    TurnStart,
    Trading,
    DevCard,
    Build,
    RobberResolve,
    TurnEnd,
    GameEnd,
    None
};

/* -----------------------
    外部接口
    ----------------------- */

// ----- 地图组 -----
// 初始化地图
extern void Map_Init(int seed);
// 地图绘制
extern void Map_Draw();
// 玩家交易属性（0元购, 4:1, 3:1, 2:1……）
extern vector<bool> Map_GetTradeOption(int playerId);
// 资源产出(ID, type, amount)
extern vector<vector<pair<ResourceType, int>>> Map_ProduceResources(int diceRoll);
extern vector<pair<ResourceType, int>> Map_PreResources(int playerId);
// 玩家建设
extern void Map_HandleBuildRequest(BuildType type, int playerId, bool isPre = false);
extern bool Map_HandleBuildRequest(BuildType type, int playerId, int mouseX, int mouseY, bool isPre = false);
// 长路特判(ID)
extern int Map_CheckLongestRoad();
// 强盗移动(ID List)
extern void Map_MoveRobber();
extern vector<int> Map_MoveRobber(int mouseX, int mouseY);

// ----- 用户组 -----
// 玩家初始化
extern void Users_RegisterPlayer(const string &name, int id);
extern void Users_Draw(int playerId);
// 资源管理
extern void Resources_Dec(int playerId, ResourceType type, int amount);
extern void Resources_Add(int playerId, vector<pair<ResourceType, int>>);
// 打出发展卡
extern void Users_PlayDevCardUI();
extern DevCardType Users_PlayDevCardUI(int playerId, int mouseX, int mouseY);
// 分数管理(胜利判定)
extern bool Score_CheckVictory(int playerId, int LongestRoadScore);
// 资源卡数量检测（amount）
extern int Resources_CheckDiscardCount(int playerId, ResourceType res);
// 丢弃资源
extern void Resources_DoDiscard(int playerId);
extern ResourceType Resources_DoDiscard(int playerId, int mouseX, int mouseY);
// 最多的骑士特判
extern int Users_CheckMostKnight();

// ----- 交易组 -----
extern void Trade_Draw(int playerId);
// 玩家购买
extern bool Trade_PlayerBuy(int playerId, int mouseX, int mouseY);
// 银行交易
extern void Trade_BankTrade(int playerId, const vector<bool> &give, bool isFree);
// 玩家间交易
extern void Trade_PlayerTradeUI(int offeringPlayer, int receivingPlayer);

// ----- UI -----
// 绘制 HUD（包括当前回合、玩家信息、按钮等）
void UI_DrawHUD(TurnPhase phase);
void UI_DiceRowing(int d1, int d2);
void UI_ShowWinner();
int UI_ChooseID(vector<int> ids);
ResourceType UI_ChooseResource();
TurnPhase UI_PlayerMove(const MouseEvent &evt);

// 获取鼠标事件
MouseEvent PollMouseEvent() {
    MouseEvent evt;
    while (MouseHit()) {
        MOUSEMSG m = GetMouseMsg();
        evt.x = m.x;
        evt.y = m.y;

        evt.leftDown  = m.uMsg == WM_LBUTTONDOWN;
        evt.rightDown = m.uMsg == WM_RBUTTONDOWN;
    }
    return evt;
}

/* -----------------------
    内部状态与工具函数
    ----------------------- */
struct GameState {
    int playerCount = 0;
    int currentPlayer = 0;
    TurnPhase phase = TurnPhase::PreGameSetup_FirstPlacement;
    bool isSettlement = true;
    int diceRoll = 0;
    bool PlayedDevCard = false;
    bool isFree = false;
    BuildType building = BuildType::None;
    bool gameRunning = true;
    unsigned int rngSeed = (unsigned int)time(nullptr);
    int victoryPlayerId = -1;
} G;

/* -----------------------
    启动与资源初始化
    ----------------------- */
void SetupGame(int numPlayers, unsigned int seed) {
    G.playerCount = numPlayers;

    for (int i = 1; i <= numPlayers; ++i) {
        string defaultName = "Player" + to_string(i);
        Users_RegisterPlayer(defaultName, i);
    }
    G.currentPlayer = 1;
    G.phase = TurnPhase::PreGameSetup_FirstPlacement;
    G.isSettlement = true;
    G.rngSeed = seed;
    Map_Init(seed);
}

/* -----------------------
    回合与阶段处理函数
    ----------------------- */

// 开局放置阶段（村庄+道路两轮）
void HandlePreGamePlacement(const MouseEvent& evt) {
    bool success =  false;
    if (G.isSettlement){
        Map_HandleBuildRequest(BuildType::Settlement, G.currentPlayer, true);
        if (evt.leftDown) {
            success = Map_HandleBuildRequest(BuildType::Settlement, G.currentPlayer, evt.x, evt.y,  true);
        }
    }else{
        Map_HandleBuildRequest(BuildType::Road, G.currentPlayer);
        if (evt.leftDown){
            success = Map_HandleBuildRequest(BuildType::Road, G.currentPlayer, evt.x, evt.y);
        }
    }

    if (success){
        if (G.isSettlement){
            G.isSettlement = false;
            const auto res = Map_PreResources(G.currentPlayer);
            Resources_Add(G.currentPlayer, res);
        } else{
            if (G.phase == TurnPhase::PreGameSetup_FirstPlacement) {
                if (G.currentPlayer + 1 <= G.playerCount) {
                    ++G.currentPlayer;
                    G.isSettlement = true;
                } else {
                    G.phase = TurnPhase::PreGameSetup_SecondPlacement;
                    G.currentPlayer = G.playerCount;
                    G.isSettlement = true;
                }
            } else {
                if (G.currentPlayer > 1) {
                    --G.currentPlayer;
                    G.isSettlement = true;
                } else {
                    G.phase = TurnPhase::DiceRoll;
                }
            }
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

// 掷骰子阶段
void HandleDiceRoll(const MouseEvent &evt){
    if (evt.leftDown) {
        const int d1 = rand() % 6 + 1;
        const int d2 = rand() % 6 + 1;
        UI_DiceRowing(d1, d2);
        G.diceRoll = d1 + d2;

        if (G.diceRoll == 7) {
            G.phase = TurnPhase::ResourceThrow;
        } else {
            G.phase = TurnPhase::ResourceDistribution;
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

// 资源分配阶段
void HandleResourceDistribution() {
    const auto prod = Map_ProduceResources(G.diceRoll);
    for (int i=1; i<=G.playerCount; ++i) {
        if (!prod[i].empty()){
            Resources_Add(i, prod[i]);
            //TODO 此处应该有炫酷的动画
        }
    }
    G.phase = TurnPhase::TurnStart;
}

// 丢卡特殊处理
void HandleResourceThrow(){
    for (int i=1; i<=G.playerCount; ++i){
        int discard = Resources_CheckDiscardCount(i, ResourceType::ALL);
        if (discard > 7) {
            discard /= 2;
            while (discard){
                Resources_DoDiscard(G.currentPlayer);
                MouseEvent evt = PollMouseEvent();

                if (evt.leftDown){
                    const auto res = Resources_DoDiscard(G.currentPlayer, evt.x, evt.y);
                    if (res != ResourceType::None){
                        Resources_Dec(i, res, 1);
                        --discard;
                    }
                }

                cleardevice();
                Map_Draw();
                Users_Draw(i);
                UI_DrawHUD(G.phase);
                FlushBatchDraw();

                Sleep(30);
            }
        }
    }

    G.phase = TurnPhase::RobberResolve;
}

// 强盗处理
void HandleRobberResolve(const MouseEvent &evt) {
    Map_MoveRobber();
    if (evt.leftDown) {
        const auto victims = Map_MoveRobber(evt.x, evt.y);
        if (!victims.empty()){
            const int victim = UI_ChooseID(victims);
            while (true){
                Resources_DoDiscard(victim);
                MouseEvent event = PollMouseEvent();

                if (event.leftDown){
                    auto res = Resources_DoDiscard(victim, event.x, event.y);
                    if (res != ResourceType::None){
                        Resources_Dec(victim, res, 1);
                        Resources_Add(G.currentPlayer, {{res, 1}});
                        break;
                    }
                }

                cleardevice();
                Map_Draw();
                Users_Draw(G.currentPlayer);
                UI_DrawHUD(G.phase);
                FlushBatchDraw();

                Sleep(30);
            }

            G.phase = TurnPhase::TurnStart;
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

void HandleBuild(const MouseEvent &evt){
    auto building = G.building;
    if (building == BuildType::DoubleRoad){
        building = BuildType::Road;
    }
    Map_HandleBuildRequest(building, G.currentPlayer);
    if (evt.leftDown){
        bool success = false;
        success = Map_HandleBuildRequest(building, G.currentPlayer, evt.x, evt.y);
        if (success){
            if (G.building == BuildType::DoubleRoad){
                G.building = BuildType::Road;
            }else{
                G.phase = TurnPhase::TurnStart;
            }
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

void HandleTrading(const MouseEvent &evt){
    if (evt.leftDown){

    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    Trade_Draw(G.currentPlayer);
    FlushBatchDraw();
}

void HandleDevCard(const MouseEvent &evt){
    Users_PlayDevCardUI();
    if (evt.leftDown){
        const auto card = Users_PlayDevCardUI(G.currentPlayer, evt.x, evt.y);
        switch (card){
            case DevCardType::Knight:{
                G.phase = TurnPhase::RobberResolve;
                break;
            }
            case DevCardType::Monopoly:{
                G.isFree = true;
                G.phase = TurnPhase::Trading;
                break;
            }
            case DevCardType::RoadBuilding:{
                G.building = BuildType::DoubleRoad;
                G.phase = TurnPhase::Build;
                break;
            }
            case DevCardType::YearOfPlenty:{
                const auto res = UI_ChooseResource();
                int cnt = 0;
                for (int i=1; i<=G.playerCount; ++i){
                    if (i != G.currentPlayer){
                        const int amt = Resources_CheckDiscardCount(i, res);
                        cnt += amt;
                        Resources_Dec(i, res, amt);
                    }
                }
                Resources_Add(G.currentPlayer, {{res, cnt}});
                G.phase = TurnPhase::TurnStart;
                break;
            }
            default:break;
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

// 玩家回合（玩家可以交易/购买/建造）
void HandleTurnStart(const MouseEvent &evt) {
    if (evt.leftDown){
        const auto act = UI_PlayerMove(evt);
        if (act != TurnPhase::None){
            G.phase = act;
        }
    }

    cleardevice();
    Map_Draw();
    Users_Draw(G.currentPlayer);
    UI_DrawHUD(G.phase);
    FlushBatchDraw();
}

// 回合结束
void HandleTurnEnd() {
    // 检查胜利
    int LRS = 0;
    if (G.currentPlayer == Map_CheckLongestRoad()){
        LRS = 2;
    }
    if (Score_CheckVictory(G.currentPlayer, LRS)) {
        G.phase = TurnPhase::GameEnd;
        G.victoryPlayerId = G.currentPlayer;
    }
    G.currentPlayer = G.currentPlayer % G.playerCount + 1;
    G.phase = TurnPhase::DiceRoll;
}

/* -----------------------
    主循环
    ----------------------- */
int main() {
    initgraph(1024, 768);
    srand((int)time(nullptr));
    BeginBatchDraw();
    SetupGame(4, (int)time(nullptr));

    while (G.gameRunning) {
        MouseEvent evt = PollMouseEvent();

        switch (G.phase) {
            case TurnPhase::PreGameSetup_FirstPlacement:
            case TurnPhase::PreGameSetup_SecondPlacement:{
                HandlePreGamePlacement(evt);
                break;
            }
            case TurnPhase::DiceRoll:
                HandleDiceRoll(evt);
                break;
            case TurnPhase::ResourceDistribution:
                HandleResourceDistribution();
                break;
            case TurnPhase::ResourceThrow:
                HandleResourceThrow();
                break;
            case TurnPhase::RobberResolve:
                HandleRobberResolve(evt);
                break;
            case TurnPhase::TurnStart:
                HandleTurnStart(evt);
                break;
            case TurnPhase::Trading:
                HandleTrading(evt);
                break;
            case TurnPhase::DevCard:
                HandleDevCard(evt);
                break;
            case TurnPhase::Build:
                HandleBuild(evt);
                break;
            case TurnPhase::TurnEnd:
                HandleTurnEnd();
                break;
            case TurnPhase::GameEnd:
                UI_ShowWinner();
                G.gameRunning = false;
                break;
            default:
                G.phase = TurnPhase::TurnStart;
                break;
        }

        Sleep(30);
    }

    EndBatchDraw();
    _getch();
    closegraph();
    return 0;
}