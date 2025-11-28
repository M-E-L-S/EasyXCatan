// RobbedResourcePanel.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "RobbedResourcePanel.h"
#include <cstdio>
#include <string>

static std::string utf8_to_ansi(const std::string &utf8) {
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

// 颜色常量（确保和目标UI一致）
#define WHITE RGB(255, 255, 255)
#define BLACK RGB(0, 0, 0)
#define SUCCESS_COLOR RGB(255, 215, 0)  // 成功金色
#define FAIL_COLOR RGB(200, 50, 50)     // 失败红色
#define BUTTON_COLOR RGB(100, 100, 255) // 按钮蓝色

std::string continuetext = utf8_to_ansi("确 定");

// 构造函数：初始化布局+按钮+抢夺结果数据
RobbedResourcePanel::RobbedResourcePanel(int w, int h, int stolenResType, int victimID)
    : width(w), height(h),
    stolenResType(stolenResType), victimID(victimID),
    hasImagesLoaded(false),
    // 初始化确认按钮（位置后续通过calculateLayout计算）
    confirmBtn(0, 0, BUTTON_W, BUTTON_H, continuetext, BUTTON_COLOR) {
    // 计算所有元素布局（弹窗+资源+按钮居中）
    calculateLayout();
    // 一次性设置按钮位置（placement new，不修改Button类）
    new (&confirmBtn) Button(confirmBtn.x, confirmBtn.y, BUTTON_W, BUTTON_H, continuetext, BUTTON_COLOR);
}

// 加载资源图片（可选）
void RobbedResourcePanel::loadResourceImages(const char* paths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (paths[i] != NULL) {
            loadimage(&resourceImages[i], paths[i], RES_ICON_SIZE_PANEL, RES_ICON_SIZE_PANEL);
        }
    }
    hasImagesLoaded = true;
}

// 加载全屏背景图（添加到 RobbedResourcePanel.cpp）
void RobbedResourcePanel::loadFullScreenBackground(const char* bgPath) {
    if (bgPath == nullptr) return;

    // 加载背景图，尺寸适配屏幕（SCREEN_WIDTH x SCREEN_HEIGHT）
    // 自动缩放图片填充全屏，避免留黑边
    loadimage(&fullScreenBgImage, bgPath, SCREEN_WIDTH, SCREEN_HEIGHT);

    // 判断加载成功（宽=屏幕宽，高=屏幕高）
    hasFullScreenBgLoaded = true;
}

// 计算所有元素居中布局（完全还原目标UI的位置逻辑）
void RobbedResourcePanel::calculateLayout() {
    // 1. 弹窗居中
    START_X = (width - DIALOG_W) / 2;
    START_Y = (height - DIALOG_H) / 2;

    // 2. 确认按钮位置（弹窗底部居中，距离底部70px）
    confirmBtn.x = START_X + (DIALOG_W - BUTTON_W) / 2;
    confirmBtn.y = START_Y + DIALOG_H - 70;
}

// 绘制深色遮罩（和目标UI一致：RGB(30,30,30)全屏遮罩）
// 修改后的 drawMask 方法（替换原实现）
void RobbedResourcePanel::drawMask() {
    if (hasFullScreenBgLoaded) {
        // 绘制全屏背景图（覆盖整个界面，最底层）
        putimage(0, 0, &fullScreenBgImage);
    }
    else {
        // 加载失败回退：原深色遮罩（保持兼容性）
        setfillcolor(RGB(30, 30, 30));
        solidrectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

// 绘制弹窗主体（背景+边框，还原目标UI的成功/失败颜色区分）
void RobbedResourcePanel::drawDialog() {
    // 1. 弹窗底板（深色RGB(50,50,50)圆角矩形）
    setfillcolor(RGB(50, 50, 50));
    solidroundrect(START_X, START_Y, START_X + DIALOG_W, START_Y + DIALOG_H, 20, 20);

    // 2. 弹窗边框（成功=金色，失败=红色，线宽5px）
    COLORREF borderColor = (stolenResType != -1) ? SUCCESS_COLOR : FAIL_COLOR;
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, 5);
    roundrect(START_X, START_Y, START_X + DIALOG_W, START_Y + DIALOG_H, 20, 20);
}

// 绘制标题+结果文字（完全还原目标UI的文字样式和位置）
void RobbedResourcePanel::drawTexts() {
    setbkmode(TRANSPARENT);
    std::string title = utf8_to_ansi("抢劫结果");
    std::string msg;

    // 1. 构建结果文字
    if (stolenResType == -1) {
        msg = "P" + std::to_string(victimID) + " 手里没有资源，扑空了！";
    }
    else {
        // 资源名称（和目标UI的switch对应）
        std::string resName;
        switch (stolenResType) {
        case 0: resName = "帝王蟹"; break;
        case 1: resName = "书籍"; break;
        case 2: resName = "汉堡"; break;
        case 3: resName = "咖啡"; break;
        case 4: resName = "研究成果"; break;
        default: resName = "未知资源"; break;
        }
        msg = "从 P" + std::to_string(victimID) + " 抢到了: " + resName;
    }

    msg=utf8_to_ansi(msg);
    std::string fontNameAnsi = utf8_to_ansi("微软雅黑");

    // 2. 绘制标题（居中，36号微软雅黑，颜色和边框一致）
    settextcolor((stolenResType != -1) ? SUCCESS_COLOR : FAIL_COLOR);
    settextstyle(36, 0, fontNameAnsi.c_str());
    int tw = textwidth(_T(title.c_str()));
    outtextxy(START_X + (DIALOG_W - tw) / 2, START_Y + 30, _T(title.c_str()));

    // 3. 绘制结果文字（居中，24号微软雅黑，白色）
    settextcolor(WHITE);
    settextstyle(36, 0, fontNameAnsi.c_str());
    tw = textwidth(_T(msg.c_str()));

    // 文字Y坐标：有资源则在图标下方，无资源则居中
    int msgY = (stolenResType != -1)
        ? (START_Y + 90 + RES_ICON_SIZE_PANEL + 5)
        : (START_Y + 120);
    outtextxy(START_X + (DIALOG_W - tw) / 2, msgY, _T(msg.c_str()));
}

// 绘制抢夺到的资源图标/色块（完全还原目标UI的资源展示逻辑）
void RobbedResourcePanel::drawResourceIcon() {
    // 仅当抢到资源时绘制（stolenResType != -1）
    if (stolenResType == -1) return;

    // 资源图标居中（水平居中，Y坐标=START_Y+90）
    int iconX = START_X + (DIALOG_W - RES_ICON_SIZE_PANEL) / 2;
    int iconY = START_Y + 90;

    if (hasImagesLoaded && resourceImages[stolenResType].getwidth() > 0) {
        // 有图片：绘制资源图片
        putimage(iconX, iconY, &resourceImages[stolenResType]);
    }
    else {
        // 无图片：绘制彩色色块+白色边框
        setfillcolor(resColors[stolenResType]);
        solidrectangle(iconX, iconY, iconX + RES_ICON_SIZE_PANEL, iconY + RES_ICON_SIZE_PANEL);

        // 白色边框增强辨识度
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 2);
        rectangle(iconX, iconY, iconX + RES_ICON_SIZE_PANEL, iconY + RES_ICON_SIZE_PANEL);
    }
}

// 绘制确认按钮（还原目标UI的蓝色按钮样式）
void RobbedResourcePanel::drawButton() {
    confirmBtn.draw();
}

// 核心：显示面板+交互逻辑（阻塞式，点击确认关闭）
void RobbedResourcePanel::show() {
    BeginBatchDraw();
    ExMessage msg;
    bool exit = false;

    while (!exit) {
        // 绘制层级（从下到上：遮罩→弹窗→标题+文字→资源图标→按钮）
        drawMask();
        drawDialog();
        drawTexts();
        drawResourceIcon();
        drawButton();

        FlushBatchDraw();

        // 处理鼠标交互（仅响应确认按钮点击）
        if (peekmessage(&msg, EM_MOUSE) && msg.message == WM_LBUTTONDOWN) {
            if (confirmBtn.isClicked(msg.x, msg.y)) {
                exit = true;
            }
        }
        Sleep(16); // 降低CPU占用
    }

    EndBatchDraw();
}