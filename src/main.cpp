#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
#include <memory>

#include "enumclass.h"
#include "map.h"
#include "BridgeToPlayer.h"
#include "PlayerLoop.h"

using namespace std;

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
    RobberAnimate,
    TurnEnd,
    GameEnd,
    None
};

int screenHeight = 0;
int screenWidth = 0;

// ----- UI -----
// 绘制 HUD（包括当前回合、玩家信息、按钮等）
void UI_DrawHUD();
void UI_DiceRowing(const int d1, const int d2);
bool UI_SwitchToPlayerPanel(const MouseEvent &evt);
int UI_ChooseID(const vector<int>& ids);
//ResourceType UI_ChooseResource();
void UI_ShowWinner();
string Users_GetPlayerName(int playerId){
    return "Player" + to_string(playerId);
}

// 获取鼠标事件
MouseEvent PollMouseEvent() {
    MouseEvent evt;
    while (MouseHit()) {
        MOUSEMSG m = GetMouseMsg();
        evt.x = m.x;
        evt.y = m.y;
        evt.hasEvent = true;
        evt.leftDown  |= m.uMsg == WM_LBUTTONDOWN;
    }
    return evt;
}

/* -----------------------
    内部状态与工具函数
    ----------------------- */
struct Game_state {
    int playerCount = 0;
    int currentPlayer = 0;
    TurnPhase phase = TurnPhase::PreGameSetup_FirstPlacement;
    bool isSettlement = true;
    int diceRoll = 0;
    //bool PlayedDevCard = false;
    //bool isFree = false;
    BuildType building = BuildType::None;
    vector<int> victims;
    bool gameRunning = true;
    unsigned int rngSeed = (unsigned int)time(nullptr);
    int victoryPlayerId = -1;
    int screenWidth = 2560;
    int screenHeight = 1600;
    IMAGE LR, MK;
    IMAGE dice[21];
    unique_ptr<Map> map;
} G;

/* -----------------------
    启动与资源初始化
    ----------------------- */
void SetupGame(int numPlayers, unsigned int seed, const IMAGE& bk) {
    G.playerCount = numPlayers;

    for (int i = 1; i <= numPlayers; ++i) {
        PlayerInit(i);
    }
    G.currentPlayer = 1;
    G.phase = TurnPhase::PreGameSetup_FirstPlacement;
    G.isSettlement = true;
    G.rngSeed = seed;
    G.map = make_unique<Map>(G.rngSeed);
    G.map->initMap(bk);
}

/* -----------------------
    回合与阶段处理函数
    ----------------------- */

// 开局放置阶段（村庄+道路两轮）
void HandlePreGamePlacement(const MouseEvent& evt) {
    cleardevice();
    G.map->drawAll();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    bool success =  false;
    if (G.isSettlement){
        G.map->handleBuildRequest(BuildingType::SETTLEMENT, G.currentPlayer, true);
        if (evt.leftDown) {
            success = G.map->handleBuildRequest(BuildingType::SETTLEMENT, G.currentPlayer, evt.x, evt.y,  true);
        }
    }else{
        G.map->handleBuildRequest(BuildingType::ROAD, G.currentPlayer);
        if (evt.leftDown){
            success = G.map->handleBuildRequest(BuildingType::ROAD, G.currentPlayer, evt.x, evt.y);
        }
    }

    if (success){
        if (G.isSettlement){
            G.isSettlement = false;
            if (G.phase == TurnPhase::PreGameSetup_SecondPlacement){
                const auto res = G.map->getPlayerResources(G.currentPlayer);
                Resources_Add(G.currentPlayer - 1, res);
            }
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
    G.map->drawAll();
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
    const auto prod = G.map->distributeResources(G.diceRoll);
    for (int i=1; i<=G.playerCount; ++i) {
        if (!prod[i].empty()){
            Resources_Add(i - 1, prod[i]);
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
    G.map->drawAll();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    G.map->handleRobberMove();
    if (evt.leftDown) {
        const auto [ok, victims] = G.map->handleRobberMove(evt.x, evt.y);
        if (ok){
            G.phase  = TurnPhase::RobberAnimate;
            G.victims = victims;
        }
    }
}

void HandleRobberAnimate(){
    cleardevice();
    G.map->drawAll();
    if (G.map->moveRobber()){
        cleardevice();
        G.map->drawAll();
        UI_DrawHUD();
        FlushBatchDraw();
        if (!G.victims.empty()){
            const int victim = UI_ChooseID(G.victims);
            Resources_Discard(G.currentPlayer,  victim);
        }
        G.phase = TurnPhase::TurnStart;
    }else{
        UI_DrawHUD();
        FlushBatchDraw();
    }
}

void HandleBuild(const MouseEvent &evt){
    cleardevice();
    G.map->drawAll();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();
    FlushBatchDraw();

    auto building = BuildingType::ROAD;
    if (G.building == BuildType::Settlement){
        building = BuildingType::SETTLEMENT;
    }else if (G.building == BuildType::City){
        building = BuildingType::CITY;
    }
    G.map->handleBuildRequest(building, G.currentPlayer);
    if (evt.leftDown){
        bool success = false;
        success = G.map->handleBuildRequest(building, G.currentPlayer, evt.x, evt.y);
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
void HandleTurnStart(const MouseEvent & evt) {
    cleardevice();
    G.map->drawAll();
    //Users_Draw(G.currentPlayer);
    UI_DrawHUD();

    if (UI_SwitchToPlayerPanel(evt)){
        auto player = getPlayer(G.currentPlayer - 1);
        const auto act = PlayerLoop(player, G.diceRoll, G.map->GetTradeOption(G.currentPlayer));
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
        // if (act != TurnPhase::None){
        //     G.phase = act;
        // }
    }
    FlushBatchDraw();
}

// 回合结束
void HandleTurnEnd() {
    // 检查胜利
    int LRS = 0;
    if (G.currentPlayer == G.map->longRoadOwner()){
        LRS = 2;
    }
    if (Score_CheckVictory(G.currentPlayer - 1, LRS) == 10) {
        G.phase = TurnPhase::GameEnd;
        G.victoryPlayerId = G.currentPlayer;
    }else{
        G.currentPlayer = G.currentPlayer % G.playerCount + 1;
        G.phase = TurnPhase::DiceRoll;
    }
    // G.PlayedDevCard = false;
}

/* -----------------------
    主循环
    ----------------------- */
int main() {
    G.screenWidth = GetSystemMetrics(SM_CXSCREEN);
    G.screenHeight = GetSystemMetrics(SM_CYSCREEN);
    screenHeight = G.screenHeight;
    screenWidth = G.screenWidth;
    initgraph(G.screenWidth, G.screenHeight);
    srand((unsigned int)time(nullptr));
    BeginBatchDraw();

    loadimage(&G.LR, "resources/image/LR.png", G.screenWidth / 10, G.screenHeight / 10);
    loadimage(&G.MK, "resources/image/MK.png", G.screenWidth / 10, G.screenHeight / 10);
    IMAGE bk;
    loadimage(&bk, "resources/image/background.png", G.screenWidth, G.screenHeight);
    char path[100] = {0};
    for(int i = 0; i < 21; i++) {
        sprintf(path, "resources/image/dice%d.png", i + 1);
        loadimage(G.dice + i, path, 100, 100);
    }

    SetupGame(4, (unsigned int)time(nullptr), bk);

    while (G.gameRunning) {
        BeginBatchDraw();
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
            case TurnPhase::RobberAnimate:
                HandleRobberAnimate();
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
    closegraph();
    return 0;
}

static string utf8_to_ansi(const std::string &utf8) {
    if (utf8.empty()) return std::string();
    // 先从 UTF-8 转为宽字符串
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (wlen == 0) return std::string();
    std::wstring wstr;
    wstr.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // 再从宽字符串转为 ANSI（CP_ACP）
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len == 0) return std::string();
    std::string ans;
    ans.resize(len);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &ans[0], len, nullptr, nullptr);
    // 去掉末尾的 '\0'（WideCharToMultiByte 返回的长度包含终止符）
    if (!ans.empty() && ans.back() == '\0') ans.pop_back();
    return ans;
}

void UI_PhaseText(const string& text, int r, int g, int b){
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
    if (G.map->longRoadOwner() == G.currentPlayer) LRS = 2;
    int score = Score_CheckVictory(G.currentPlayer - 1, LRS);

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
    outtextxy(G.screenWidth / 50, G.screenHeight - fontHeight * 2 - G.screenHeight / 50, nameTextAnsi.c_str());
}

void UI_DrawHUD(){
    switch (G.phase){
        case TurnPhase::PreGameSetup_FirstPlacement:
        case TurnPhase::PreGameSetup_SecondPlacement:
            UI_PhaseText("入场阶段", 108,214,184);
            break;
        case TurnPhase::DiceRoll:
            UI_PhaseText("投骰子", 66,126,252);
            putimage((G.screenWidth - 300) / 2, (G.screenHeight - 100) / 2, &G.dice[20]);
            putimage((G.screenWidth - 300) / 2 + 200, (G.screenHeight - 100) / 2, &G.dice[20]);
            break;
        case TurnPhase::RobberResolve:
            UI_PhaseText("强盗来袭！", 192,0,0);
            break;
        case TurnPhase::Build:
            UI_PhaseText("建造阶段", 108,214,184);
            break;
        case TurnPhase::TurnStart:
            UI_PhaseText("回合中……",235,184,36);
            break;
        case TurnPhase::RobberAnimate:
            UI_PhaseText("奶龙移动中……",192,0,0);
            break;
        default:
            UI_PhaseText("……",235,184,36);
            break;
    }
    UI_ScoreBoard();
    UI_PlayerName();
}

struct UI_Button {
    int id;
    int x, y, w, h;
    bool isHover;
    string text;
};

int UI_ChooseID(const vector<int>& ids){
    const int BTN_W = G.screenWidth / 10;
    const int BTN_H = G.screenHeight / 15;
    const int BTN_X = G.screenWidth / 50;
    const int START_Y = G.screenHeight / 5;
    const int GAP = G.screenHeight / 75;

    constexpr COLORREF COLOR_NORMAL = RGB(219, 195, 147);
    constexpr COLORREF COLOR_HOVER  = RGB(255, 200, 80);
    constexpr COLORREF COLOR_BORDER = RGB(50, 50, 50);

    vector<UI_Button> buttons;
    for (int i = 0; i < ids.size(); i++){
        UI_Button btn;
        btn.id = ids[i];
        btn.x = BTN_X;
        btn.y = START_Y + i * (BTN_H + GAP);
        btn.w = BTN_W;
        btn.h = BTN_H;
        btn.isHover = false;
        btn.text = utf8_to_ansi(Users_GetPlayerName(ids[i]));
        buttons.push_back(btn);
    }

    setbkmode(TRANSPARENT);
    settextcolor(RED);
    settextstyle(30, 0, utf8_to_ansi("微软雅黑").c_str());
    outtextxy(BTN_X, START_Y - 50, utf8_to_ansi("选择抢夺对象：").c_str());

    int choice = 0;
    int x = 0, y = 0;

    while (!choice){
        MouseEvent msg = PollMouseEvent();
        if (msg.hasEvent){
            x = msg.x;
            y = msg.y;
        }
        for (auto& btn : buttons){
            btn.isHover = x >= btn.x && x < btn.x + btn.w && y >= btn.y && y < btn.y + btn.h;
            if (btn.isHover && msg.leftDown){
                choice = btn.id;
                break;
            }
        }

        for (const auto& btn : buttons){
            if (btn.isHover){
                setfillcolor(COLOR_HOVER);
            }
            else{
                setfillcolor(COLOR_NORMAL);
            }
            setlinecolor(COLOR_BORDER);
            setlinestyle(PS_SOLID, 2);

            fillroundrect(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, 10, 10);

            settextcolor(BLACK);
            settextstyle(24, 0, utf8_to_ansi("华文新魏").c_str());
            int tw = textwidth(btn.text.c_str());
            int th = textheight(btn.text.c_str());
            int tx = btn.x + (btn.w - tw) / 2;
            int ty = btn.y + (btn.h - th) / 2;
            outtextxy(tx, ty, btn.text.c_str());
        }

        FlushBatchDraw();
        Sleep(30);
    }
    return choice;
}

bool UI_SwitchToPlayerPanel(const MouseEvent & evt){
    const int BTN_W = G.screenWidth / 8;
    const int BTN_H = G.screenHeight / 10;
    constexpr int MARGIN = 50;

    int x = G.screenWidth - BTN_W * 1.25 - MARGIN;
    int y = G.screenHeight - BTN_H * 1.5 - MARGIN;

    static int mx = 0, my = 0;

    if (evt.hasEvent){
        mx = evt.x;
        my = evt.y;
    }
    bool isHover = (mx >= x && mx <= x + BTN_W && my >= y && my <= y + BTN_H);

    if (isHover)
        setfillcolor(RGB(255, 200, 80));
    else
        setfillcolor(RGB(219, 195, 147));

    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 2);

    fillroundrect(x, y, x + BTN_W, y + BTN_H, 15, 15);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(G.screenHeight / 25, 0, utf8_to_ansi("华文新魏").c_str());

    string text = utf8_to_ansi("个人界面");
    int tx = x + (BTN_W - textwidth(text.c_str())) / 2;
    int ty = y + (BTN_H - textheight(text.c_str())) / 2;
    outtextxy(tx, ty, text.c_str());

    return isHover && evt.leftDown;
}

void UI_DiceRowing(const int d1, const int d2){
    int currentFrame = 0;
    const int x = (G.screenWidth - 300) / 2;
    const int y = (G.screenHeight - 100) / 2;
    while(currentFrame <= 29) {
        const int t = currentFrame % 15;
        cleardevice();
        G.map->drawAll();
        UI_DrawHUD();
        putimage(x, y, G.dice + t);
        putimage(x + 200, y, G.dice + t);
        FlushBatchDraw();

        ++currentFrame;
        Sleep(30);
    }
    while (true){
        const MouseEvent evt = PollMouseEvent();
        if (evt.leftDown) break;
        cleardevice();
        G.map->drawAll();
        UI_DrawHUD();
        putimage(x, y, G.dice + 14 + d1);
        putimage(x + 200, y, G.dice + 14 + d2);
        string text = to_string(d1+d2) + "!";
        setbkmode(TRANSPARENT);
        settextcolor(RGB(251,210,84));
        settextstyle(G.screenHeight / 10, 0, utf8_to_ansi("方正姚体").c_str(), 0, 0, FW_BOLD, false, false,  false);
        outtextxy(G.screenWidth / 30, y, text.c_str());
        FlushBatchDraw();

        Sleep(30);
    }
}

void smooth_action(int startW, int startH, int duration = 1000) {
    int w = G.screenWidth;
    int h = G.screenHeight;
    int totalFrames = duration / 8; // 这里的帧数计算可能导致循环次数较少，建议 duration / 16 或直接用时间差

    // 静态资源加载
    static IMAGE img_bk;
    static IMAGE card;
    static IMAGE vic;
    static bool isLoaded = false;
    if (!isLoaded) {
        loadimage(&img_bk, "resources/image/background.jpg", w, h);
        loadimage(&card, "resources/image/card.jpg", 500, 500);
        loadimage(&vic, "resources/image/victory.jpg", w,h);
        isLoaded = true;
        printf("OK\n");
    }

    IMAGE buffer;
    buffer.Resize(w, h); // 假设你的库支持这个Resize，如果报错请用 Resize(&buffer, w, h) 或重新 loadimage

    for (int frame = 0; frame < totalFrames; frame++) {
        // 1. 设置绘图目标为缓冲图片
        SetWorkingImage(&buffer);
        cleardevice();

        // 2. 计算缓动参数
        float progress = (float)frame / totalFrames;
        float easeProgress = 1.0f - pow(1.0f - progress, 3); // Cubic Ease Out
        int cur_w = startW + (int)((w - startW) * easeProgress);
        int cur_h = startH + (int)((h - startH) * easeProgress);

        int x = (w - cur_w) / 2;
        int y = (h - cur_h) / 2;

        // 3. 绘制背景和卡片
        putimage(0, 0, &img_bk);
        putimage((w - 500) / 2, (h - 500) / 2, &card);

        IMAGE scaledImg;
        loadimage(&scaledImg, "resources/image/victory.jpg", cur_w, cur_h);
        putimage(x, y, &scaledImg);

        // 5. 切换回屏幕并显示缓冲内容
        SetWorkingImage(NULL);
        putimage(0, 0, &buffer);

        FlushBatchDraw();

        // 你的代码里 Sleep(30) 会导致动画偏慢，
        // 1000ms / 30ms ≈ 33帧，但你计算的 totalFrames = 1000/8 = 125帧
        // 125 * 30ms = 3.75秒，动画会比预期的1秒慢很多。建议改为 Sleep(10)
        Sleep(10);
    }

    MCIERROR ret = mciSendString(_T("open \"resources/sound/victory_music.mp3\" alias victory_music"), NULL, 0, NULL);
    if (ret != 0) {
        TCHAR msg[100];
        mciGetErrorString(ret, msg, 100);
        _tprintf(_T("音乐播放错误: %s\n"), msg);
    } else {
        mciSendString(_T("play victory_music"), NULL, 0, NULL);
    }
    int cnt = 133;
    while (cnt--){
        putimage(0, 0, &img_bk);
        putimage((w - 500) / 2, (h - 500) / 2, &card);
        putimage(0,0,&vic);
        FlushBatchDraw();

        Sleep(30);
    }
    mciSendString(_T("close victory_music"), NULL, 0, NULL);
}
void UI_ShowWinner() {
    setbkcolor(RGB(164,115,56));
    cleardevice();
    IMAGE img_bk;

    // 从100x100的小图片逐渐放大到屏幕尺寸
    smooth_action(100, 100, 1000);

    while (true){
        MouseEvent evt = PollMouseEvent();
        if (evt.leftDown) break;
        Sleep(30);
    }
}