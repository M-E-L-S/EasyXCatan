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
enum class ResourceType { Wood, Brick, Ore, Sheep, Wheat, None };
enum class BuildType { Road, Settlement, City, DoubleRoad, None };
//enum class TradeType { Bank, Player, Port, None };
//enum class DevCardType { Knight, VictoryPoint, Monopoly, RoadBuilding, YearOfPlenty, None };
enum class ActionType { SwitchToMap, BuildRoad, BuildSettlement, BuildCity, Knight, RoadBuilding, EndTurn, None};

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
    //ResourceThrow,
    TurnStart,
    //Trading,
    //DevCard,
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
// 玩家交易属性（4:1, 3:1, 2:1木、砖、矿、羊、麦）（7位0-6）
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
extern void PlayerInit(int playerId);
// extern void Users_RegisterPlayer(const string &name, int id);
// extern void Users_Draw(int playerId);
// 主接口
extern ActionType PlaterPanel(int playerId, vector<bool> tradeOption);
// 资源管理
extern void Resources_Add(int playerId, vector<pair<ResourceType, int>>);
// extern void Resources_Dec(int playerId, ResourceType type, int amount);
// // 打出发展卡
// extern void Users_PlayDevCardUI();
// extern DevCardType Users_PlayDevCardUI(int playerId, int mouseX, int mouseY);
// 分数管理(胜利判定)
extern int Score_CheckVictory(int playerId, int LongestRoadScore);
// // 资源卡数量检测（amount）
// extern int Resources_CheckDiscardCount(int playerId, ResourceType res);
// 丢弃资源
extern void Resources_Discard(); //强盗丢卡
extern void Resources_Discard(int playerId, int victim); //玩家抽资源
// extern void Resources_DoDiscard(int playerId);
// extern ResourceType Resources_DoDiscard(int playerId, int mouseX, int mouseY);
// 最多的骑士特判
extern int Users_CheckMostKnight();
// 获取当前玩家昵称
extern string Users_GetPlayerName(int playerId);
//
// // ----- 交易组 -----
// extern void Trade_Draw(int playerId);
// // 玩家购买
// extern bool Trade_PlayerBuy(int playerId, int mouseX, int mouseY);
// // 银行交易
// extern void Trade_BankTrade(int playerId, const vector<bool> &give, bool isFree);
// // 玩家间交易
// extern void Trade_PlayerTradeUI(int offeringPlayer, int receivingPlayer);

// ----- UI -----
// 绘制 HUD（包括当前回合、玩家信息、按钮等）
void UI_DrawHUD();
void UI_DiceRowing(int d1, int d2);
bool UI_SwitchToPlayerPanel(const MouseEvent &evt);
int UI_ChooseID(vector<int> ids);
//ResourceType UI_ChooseResource();
void UI_ShowWinner();

// 获取鼠标事件
MouseEvent PollMouseEvent() {
    MouseEvent evt;
    while (MouseHit()) {
        MOUSEMSG m = GetMouseMsg();
        evt.x = m.x;
        evt.y = m.y;
        evt.leftDown  = m.uMsg == WM_LBUTTONDOWN;
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
    //bool PlayedDevCard = false;
    //bool isFree = false;
    BuildType building = BuildType::None;
    bool gameRunning = true;
    unsigned int rngSeed = (unsigned int)time(nullptr);
    int victoryPlayerId = -1;
    int screenWidth = 2560;
    int screenHeight = 1600;
    IMAGE LR, MK;
} G;

/* -----------------------
    启动与资源初始化
    ----------------------- */
void SetupGame(int numPlayers, unsigned int seed) {
    G.playerCount = numPlayers;

    for (int i = 1; i <= numPlayers; ++i) {
        PlayerInit(i);
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
    cleardevice();
    Map_Draw();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

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
}

// 掷骰子阶段
void HandleDiceRoll(const MouseEvent &evt){
    cleardevice();
    Map_Draw();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    if (evt.leftDown) {
        const int d1 = rand() % 6 + 1;
        const int d2 = rand() % 6 + 1;
        UI_DiceRowing(d1, d2);
        G.diceRoll = d1 + d2;

        if (G.diceRoll == 7) {
            Resources_Discard();
            G.phase = TurnPhase::RobberResolve;
        } else {
            G.phase = TurnPhase::ResourceDistribution;
        }
    }
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

// // 丢卡特殊处理
// void HandleResourceThrow(){
//     for (int i=1; i<=G.playerCount; ++i){
//         int discard = Resources_CheckDiscardCount(i, ResourceType::ALL);
//         if (discard > 7) {
//             discard /= 2;
//             while (discard){
//                 Resources_DoDiscard(i);
//                 MouseEvent evt = PollMouseEvent();
//
//                 if (evt.leftDown){
//                     const auto res = Resources_DoDiscard(i, evt.x, evt.y);
//                     if (res != ResourceType::None){
//                         Resources_Dec(i, res, 1);
//                         --discard;
//                     }
//                 }
//
//                 cleardevice();
//                 Map_Draw();
//                 Users_Draw(i);
//                 UI_DrawHUD(G.phase);
//                 FlushBatchDraw();
//
//                 Sleep(30);
//             }
//         }
//     }
//
//     G.phase = TurnPhase::RobberResolve;
// }

// 强盗处理
void HandleRobberResolve(const MouseEvent &evt) {
    cleardevice();
    Map_Draw();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    Map_MoveRobber();
    if (evt.leftDown) {
        const auto victims = Map_MoveRobber(evt.x, evt.y);
        if (!victims.empty()){
            const int victim = UI_ChooseID(victims);

            Resources_Discard(G.currentPlayer,  victim);
            // while (true){
            //     Resources_DoDiscard(victim);
            //     MouseEvent event = PollMouseEvent();
            //
            //     if (event.leftDown){
            //         auto res = Resources_DoDiscard(victim, event.x, event.y);
            //         if (res != ResourceType::None){
            //             Resources_Dec(victim, res, 1);
            //             Resources_Add(G.currentPlayer, {{res, 1}});
            //             break;
            //         }
            //     }
            //
            //     cleardevice();
            //     Map_Draw();
            //     Users_Draw(G.currentPlayer);
            //     UI_DrawHUD(G.phase);
            //     FlushBatchDraw();
            //
            //     Sleep(30);
            // }

            G.phase = TurnPhase::TurnStart;
        }
    }
}

void HandleBuild(const MouseEvent &evt){
    cleardevice();
    Map_Draw();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

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
}

// void HandleTrading(const MouseEvent &evt){
//     if (evt.leftDown){
//
//     }
//
//     cleardevice();
//     Map_Draw();
//     Users_Draw(G.currentPlayer);
//     Trade_Draw(G.currentPlayer);
//     FlushBatchDraw();
// }

// void HandleDevCard(const MouseEvent &evt){
//     Users_PlayDevCardUI();
//     if (evt.leftDown){
//         const auto card = Users_PlayDevCardUI(G.currentPlayer, evt.x, evt.y);
//         switch (card){
//             case DevCardType::Knight:{
//                 G.phase = TurnPhase::RobberResolve;
//                 G.PlayedDevCard = true;
//                 break;
//             }
//             case DevCardType::Monopoly:{
//                 G.isFree = true;
//                 G.phase = TurnPhase::Trading;
//                 G.PlayedDevCard = true;
//                 break;
//             }
//             case DevCardType::RoadBuilding:{
//                 G.building = BuildType::DoubleRoad;
//                 G.phase = TurnPhase::Build;
//                 G.PlayedDevCard = true;
//                 break;
//             }
//             case DevCardType::YearOfPlenty:{
//                 const auto res = UI_ChooseResource();
//                 int cnt = 0;
//                 for (int i=1; i<=G.playerCount; ++i){
//                     if (i != G.currentPlayer){
//                         const int amt = Resources_CheckDiscardCount(i, res);
//                         cnt += amt;
//                         Resources_Dec(i, res, amt);
//                     }
//                 }
//                 Resources_Add(G.currentPlayer, {{res, cnt}});
//                 G.phase = TurnPhase::TurnStart;
//                 G.PlayedDevCard = true;
//                 break;
//             }
//             default:break;
//         }
//     }
//
//     cleardevice();
//     Map_Draw();
//     //Users_Draw(G.currentPlayer);
//     UI_DrawHUD(G.phase);
//     FlushBatchDraw();
// }

// 玩家回合（玩家可以交易/购买/建造）
void HandleTurnStart(const MouseEvent &evt) {
    cleardevice();
    Map_Draw();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    if (evt.leftDown){
        const bool panel = UI_SwitchToPlayerPanel(evt);

        if (panel){
            const auto act = PlaterPanel(G.currentPlayer, Map_GetTradeOption(G.currentPlayer));
            switch (act){
                case ActionType::BuildRoad:
                    G.building = BuildType::Road;
                    G.phase = TurnPhase::Build;
                    break;
                case ActionType::BuildSettlement:
                    G.building = BuildType::Settlement;
                    G.phase = TurnPhase::Build;
                    break;
                case ActionType::BuildCity:
                    G.building = BuildType::City;
                    G.phase = TurnPhase::Build;
                    break;
                case ActionType::RoadBuilding:
                    G.building = BuildType::DoubleRoad;
                    G.phase = TurnPhase::Build;
                    break;
                case ActionType::Knight:
                    G.phase = TurnPhase::RobberResolve;
                    break;
                case ActionType::EndTurn:
                    G.phase = TurnPhase::TurnEnd;
                    break;
                default:
                    G.phase = TurnPhase::TurnStart;
                    break;
            }
        }
        // if (act != TurnPhase::None){
        //     G.phase = act;
        // }
    }
}

// 回合结束
void HandleTurnEnd() {
    // 检查胜利
    int LRS = 0;
    if (G.currentPlayer == Map_CheckLongestRoad()){
        LRS = 2;
    }
    if (Score_CheckVictory(G.currentPlayer, LRS) == 10) {
        G.phase = TurnPhase::GameEnd;
        G.victoryPlayerId = G.currentPlayer;
    }
    G.currentPlayer = G.currentPlayer % G.playerCount + 1;
    G.phase = TurnPhase::DiceRoll;
    // G.PlayedDevCard = false;
}

/* -----------------------
    主循环
    ----------------------- */
int main() {
    G.screenWidth = GetSystemMetrics(SM_CXSCREEN);
    G.screenHeight = GetSystemMetrics(SM_CYSCREEN);
    initgraph(G.screenWidth, G.screenHeight);
    srand((int)time(nullptr));
    BeginBatchDraw();
    SetupGame(4, (int)time(nullptr));

    loadimage(&G.LR, "resources/image/LR.png", G.screenWidth / 10, G.screenHeight / 10);
    loadimage(&G.MK, "resources/image/MK.png", G.screenWidth / 10, G.screenHeight / 10);

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
            // case TurnPhase::ResourceThrow:
            //     HandleResourceThrow();
            //     break;
            case TurnPhase::RobberResolve:
                HandleRobberResolve(evt);
                break;
            case TurnPhase::TurnStart:
                HandleTurnStart(evt);
                break;
            // case TurnPhase::Trading:
            //     HandleTrading(evt);
            //     break;
            // case TurnPhase::DevCard:
            //     HandleDevCard(evt);
            //     break;
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

static string utf8_to_ansi(const std::string &utf8) {
    if (utf8.empty()) return std::string();
    // 先从 UTF-8 转为宽字符串
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    if (wlen == 0) return std::string();
    std::wstring wstr;
    wstr.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // 再从宽字符串转为 ANSI（CP_ACP）
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (len == 0) return std::string();
    std::string ans;
    ans.resize(len);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &ans[0], len, NULL, NULL);
    // 去掉末尾的 '\0'（WideCharToMultiByte 返回的长度包含终止符）
    if (!ans.empty() && ans.back() == '\0') ans.pop_back();
    return ans;
}

void UI_PhaseText(const string text, int r, int g, int b){
    settextcolor(RGB(r,g,b));
    int fontHeight = G.screenHeight / 15;
    string fontNameAnsi = utf8_to_ansi("方正姚体");
    settextstyle(fontHeight, 0, fontNameAnsi.c_str(), 0, 0, FW_BOLD, false, false,  false);
    setbkmode(TRANSPARENT);

    string textAnsi = utf8_to_ansi(text);
    outtextxy(G.screenWidth / 50, G.screenHeight / 50, textAnsi.c_str());
}

void UI_ScoreBoard(){
    string scoreText = "得分！";
    int LRS = 0;
    bool MKS = Users_CheckMostKnight() == G.currentPlayer;
    if (Map_CheckLongestRoad() == G.currentPlayer) LRS = 2;
    int score = Score_CheckVictory(G.currentPlayer, LRS);

    settextcolor(WHITE);
    int fontHeight = G.screenHeight / 20;
    string fontNameAnsi = utf8_to_ansi("华文新魏");
    settextstyle(fontHeight, 0, fontNameAnsi.c_str(), 0, 0, 0, false, false,  false);
    setbkmode(TRANSPARENT);
    string scoreTextAnsi = utf8_to_ansi(scoreText);
    int x = G.screenWidth - G.screenWidth / 50 - fontHeight * 5;
    outtextxy(x, G.screenHeight / 25, scoreTextAnsi.c_str());

    settextcolor(RGB(94,147,255));
    fontHeight = G.screenHeight / 15;
    fontNameAnsi = utf8_to_ansi("方正姚体");
    settextstyle(fontHeight, 0, fontNameAnsi.c_str(), 0, 0, FW_BOLD, false, false,  false);
    scoreTextAnsi = utf8_to_ansi(to_string(score));
    outtextxy(G.screenWidth - G.screenWidth / 50 - fontHeight, G.screenHeight / 50, scoreTextAnsi.c_str());

    if (LRS){
        putimage(x, G.screenHeight / 25 + fontHeight, &G.LR);
    }
    if (MKS){
        putimage(G.screenWidth - G.screenWidth / 50 - fontHeight * 2, G.screenHeight / 25 + fontHeight, &G.MK);
    }
}

void UI_PlayerName(){
    string nameText = Users_GetPlayerName(G.currentPlayer);
    nameText += "的回合";
    settextcolor(RGB(232,232,232));
    int fontHeight = G.screenHeight / 20;
    string fontNameAnsi = utf8_to_ansi("华文新魏");
    settextstyle(fontHeight, 0, fontNameAnsi.c_str(), 0, 0, FW_BOLD, false, false,  false);
    string nameTextAnsi = utf8_to_ansi(nameText);
    outtextxy((G.screenWidth - textwidth(nameTextAnsi.c_str())) / 2, G.screenHeight - fontHeight * 2 - G.screenHeight / 50, nameTextAnsi.c_str());
}

void UI_DrawHUD(){
    switch (G.phase){
        case TurnPhase::PreGameSetup_FirstPlacement:
        case TurnPhase::PreGameSetup_SecondPlacement:
            UI_PhaseText("入场阶段", 108,214,184);
            UI_ScoreBoard();
            UI_PlayerName();
            break;
        case TurnPhase::DiceRoll:
            UI_PhaseText("投骰子", 108,214,184);
            UI_ScoreBoard();
            UI_PlayerName();
    }
}