// PlayerPanel.h
// 该部分没有循环能力，只能被调用画图
#pragma once
#include <vector>
#include <map>
#include <string>
#include <graphics.h> // 必须包含，用于使用 IMAGE 对象
#include "Button.h"
#include "Player.h"       // 需要 Player 来显示数据
#include "Common.h"       // 需要 Common 来获取资源枚举

class PlayerPanel {
public:

    // 构造函数
    PlayerPanel(int screenWidth, int screenHeight);

    // --- 加载资源图片的接口 ---
    // 请在游戏初始化时调用此函数，传入包含5个文件路径的数组
    // 顺序必须对应：WOOD, BRICK, SHEEP, WHEAT, ORE
    void loadResourceImages(const char* filePaths[RESOURCE_COUNT]);

    /*
     * 绘制整个全屏面板 (GameController 每一帧都会调用)
     * player 当前玩家 (用于显示资源、决定按钮是否灰显)
     * state  当前游戏状态 (用于显示 "请在地图上选点")
     * lastDiceResult 掷色子的结果。需要在GameController里更改player信息！！！
     */
    void draw(const Player& player, GameState state, int lastDiceResult);

    /*
     * @brief 检查鼠标点击 (GameController 在收到点击时调用)
     * @return 被点击的按钮类型 (ButtonType)
     */
    ButtonTypeFromPanel getClickedButton(int mouseX, int mouseY);

private:
    // --- 绘制辅助函数 ---
    void drawResources(const Player& player);
    void drawDevCards(const Player& player);
    void drawStatusMessage(GameState state, int lastDiceResult);
    void drawButtons(const Player& player, GameState state);
    void drawPlayerInfo(const Player& player);

    // --- 屏幕尺寸 ---
    int width;
    int height;

    // --- 图片资源 ---
    // 存储5种资源的图片对象
    IMAGE resourceImages[RESOURCE_COUNT];
    bool hasImagesLoaded; // 标记是否已加载图片
    // 准备好图片的路径 (确保图片在项目目录下)
    // 在 main.cpp 或 GameController 初始化 PlayerPanel 的地方，调用下面的代码：
    /*const char* resPaths[RESOURCE_COUNT] = {
        "./assets/wood.jpg",
        "./assets/brick.jpg",
        "./assets/sheep.jpg",
        "./assets/wheat.jpg",
        "./assets/ore.jpg"
    };
    PlayerPanel myPanel(1024, 768);
    myPanel.loadResourceImages(resPaths); // 这一步会自动加载并缩放图片

    如果没有调用这个函数，或者图片路径不对，程序会默认显示带颜色的色块，不会崩溃。*/

    // --- UI 控件 ---
    // 我们把所有按钮都存在一个 vector 里
    std::vector<Button> buttons;
    // 我们用一个 map 来快速查找按钮，这比遍历 vector 要高效
    std::map<ButtonTypeFromPanel, Button*> buttonMap;
};
