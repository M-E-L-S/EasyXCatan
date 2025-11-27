// DiscardPanel.h
#pragma once
#include "Common.h"
#include "Player.h"
#include "Button.h"
#include <graphics.h>
#include <string>

class DiscardPanel {
private:
    // 核心成员变量
    int width, height;       // 窗口宽高
    Player& player;          // 关联的玩家
    int selectedArray[RESOURCE_COUNT]; // 选中弃牌的资源数组
    IMAGE resourceImages[RESOURCE_COUNT]; // 资源图片
    bool hasImagesLoaded;    // 是否加载图片
    IMAGE fullScreenBgImage;  // 全屏背景图
    bool hasFullScreenBgLoaded;  // 是否加载成功

    // 美观UI布局常量（完全保留）
    const int DIALOG_W = 740;    // 对话框宽度
    const int DIALOG_H = 450;    // 对话框高度
    const int ICON_SIZE = 100;   // 资源图标大小
    const int ICON_GAP = 30;     // 资源图标间距
    int START_X;                // 对话框起始X（水平居中）
    int START_Y;                // 对话框起始Y（垂直居中）
    int RES_START_X;            // 资源图标起始X（对话框内居中）
    const int RES_START_Y = 430; // 资源图标起始Y（对话框内）

    // Button对象（仅用现有构造函数，不修改Button类）
    Button confirmBtn; // 确认弃牌（绿色）
    Button cancelBtn;  // 重新选择（红色）

    // 自己维护按钮状态（不依赖Button类的任何状态方法）
    bool isConfirmBtnEnabled; // 确认按钮是否启用
    int requiredDiscard;      // 需要弃牌的总数
    int remaining;            // 剩余需弃牌数

    // 私有辅助函数
    void calculateLayout();    // 计算所有元素布局（一次性算好按钮位置）
    void drawMask();           // 绘制深色遮罩
    void drawDialog();         // 绘制对话框主体
    void drawTexts();          // 绘制标题和提示文字
    void drawResources();      // 绘制资源图标（美观布局）
    void drawButtons();        // 绘制按钮（模拟启用/禁用）
    void updateButtonStates(); // 更新按钮状态（自己维护）
    bool isResourceClicked(int mouseX, int mouseY, int& resIndex); // 判断资源块点击
    int getSelectedTotal();    // 计算已选中资源总数
    void resetSelected();      // 重置选中状态

public:
    DiscardPanel(int w, int h, Player& p); // 构造函数（一次性初始化Button位置）
    void loadResourceImages(const char* paths[RESOURCE_COUNT]);
    // 新增：加载全屏背景图（和 RobbedResourcePanel 接口一致）
    void loadFullScreenBackground(const char* bgPath);
    bool show(); // 显示面板，返回弃牌是否成功
};