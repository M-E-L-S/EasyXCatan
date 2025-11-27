// DiscardPanel.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "DiscardPanel.h"
#include <cstdio>
#include <string>

// 颜色常量（避免未定义，不依赖外部）
#define YELLOW RGB(255, 255, 0)
#define DARKGRAY RGB(100, 100, 100)
#define GREEN RGB(0, 255, 0)
#define WHITE RGB(255, 255, 255)
#define GRAY RGB(169, 169, 169)
#define DARK_TEXT RGB(128, 128, 128)
#define BTN_DISABLE_MASK RGB(169, 169, 169, 180) // 按钮禁用遮罩（半透明）

// 构造函数：一次性初始化所有元素（Button位置一次性算好，不后续修改）
DiscardPanel::DiscardPanel(int w, int h, Player& p)
    : width(w), height(h), player(p),
    // 先计算所有布局（关键：一次性算好按钮位置，传给Button构造函数）
    requiredDiscard(0), remaining(0), isConfirmBtnEnabled(false),
    hasImagesLoaded(false),
    // 一次性初始化Button位置（通过构造函数，不使用setPosition）
    confirmBtn(0, 0, 120, 50, "确认弃牌", RGB(34, 139, 34)), // 绿色按钮
    cancelBtn(0, 0, 120, 50, "重新选择", RGB(200, 50, 50))  // 红色按钮
{
    // 初始化选中数组为0
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        selectedArray[i] = 0;
    }
    // 计算布局（对话框、资源、按钮位置）
    calculateLayout();
    // 重新初始化Button（用算好的位置，一次性到位）
    new (&confirmBtn) Button(confirmBtn.x, confirmBtn.y, 120, 50, "确认弃牌", RGB(34, 139, 34));
    new (&cancelBtn) Button(cancelBtn.x, cancelBtn.y, 120, 50, "重新选择", RGB(200, 50, 50));
}

// 加载资源图片（无图时显示美观纯色块）
void DiscardPanel::loadResourceImages(const char* paths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (paths[i] != NULL) {
            loadimage(&resourceImages[i], paths[i], ICON_SIZE, ICON_SIZE);
        }
    }
    hasImagesLoaded = true;
}
// 加载全屏背景图（添加到 DiscardPanel.cpp 中）
void DiscardPanel::loadFullScreenBackground(const char* bgPath) {
    if (bgPath == nullptr) return;

    // 加载背景图，尺寸适配屏幕（SCREEN_WIDTH x SCREEN_HEIGHT）
    loadimage(&fullScreenBgImage, bgPath, SCREEN_WIDTH, SCREEN_HEIGHT);

    // 判断加载成功（宽=屏幕宽，高=屏幕高）
    hasFullScreenBgLoaded = true;
}

// 计算所有布局（关键：一次性算好Button的最终位置，不后续修改）
void DiscardPanel::calculateLayout() {
    // 1. 对话框居中
    START_X = (width - DIALOG_W) / 2;
    START_Y = (height - DIALOG_H) / 2;

    // 2. 资源图标居中（对话框内）
    int totalResWidth = (RESOURCE_COUNT * ICON_SIZE) + ((RESOURCE_COUNT - 1) * ICON_GAP);
    RES_START_X = START_X + (DIALOG_W - totalResWidth) / 2;

    // 3. 按钮位置（对话框底部居中，并排间距20px）
    int btnY = START_Y + 350; // 按钮Y坐标
    int btnTotalWidth = 120 + 20 + 120; // 两个按钮+间距总宽
    int btnStartX = START_X + (DIALOG_W - btnTotalWidth) / 2; // 按钮组居中

    // 给Button赋值最终位置（一次性算好，不修改Button类）
    confirmBtn.x = btnStartX;
    confirmBtn.y = btnY;
    cancelBtn.x = btnStartX + 120 + 20;
    cancelBtn.y = btnY;
}

// 修改后的 drawMask 方法（替换原实现）
void DiscardPanel::drawMask() {
    if (hasFullScreenBgLoaded) {
        // 绘制全屏背景图（最底层，覆盖整个界面）
        putimage(0, 0, &fullScreenBgImage);
    }
    else {
        // 加载失败回退：原深色遮罩（保持兼容性）
        setfillcolor(RGB(30, 30, 30));
        solidrectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

// 绘制对话框主体（深色背景+彩色边框，保留美观设计）
void DiscardPanel::drawDialog() {
    // 对话框背景（深色圆角）
    setfillcolor(RGB(50, 50, 50));
    solidroundrect(START_X, START_Y, START_X + DIALOG_W, START_Y + DIALOG_H, 25, 25);

    // 对话框边框（默认美观绿色，无getColor()则用这个）
    COLORREF borderColor = RGB(60, 179, 113);
    // 若Player有getColor()，可替换为：borderColor = player.getColor();
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, 6);
    roundrect(START_X, START_Y, START_X + DIALOG_W, START_Y + DIALOG_H, 25, 25);
}

// 绘制标题和提示文字（中文适配，美观排版）
void DiscardPanel::drawTexts() {
    setbkmode(TRANSPARENT);

    // 标题（居中）
    settextcolor(WHITE);
    settextstyle(32, 0, _T("微软雅黑"));
    std::string title = "玩家" + std::to_string(player.getID()) + " - 资源弃牌";
    int tw = textwidth(_T(title.c_str()));
    outtextxy(START_X + (DIALOG_W - tw) / 2, START_Y + 30, _T(title.c_str()));

    // 提示文字（动态变化）
    if (remaining > 0) {
        settextcolor(RGB(255, 100, 100)); // 亮红色
        settextstyle(24, 0, _T("微软雅黑"));
        std::string tip = "请弃掉 " + std::to_string(remaining) + " 张（共需 " + std::to_string(requiredDiscard) + " 张）";
        tw = textwidth(_T(tip.c_str()));
        outtextxy(START_X + (DIALOG_W - tw) / 2, START_Y + 80, _T(tip.c_str()));
    }
    else {
        settextcolor(GREEN); // 绿色完成提示
        settextstyle(24, 0, _T("微软雅黑"));
        std::string okMsg = "已完成弃牌，请点击确认";
        tw = textwidth(_T(okMsg.c_str()));
        outtextxy(START_X + (DIALOG_W - tw) / 2, START_Y + 300, _T(okMsg.c_str()));
    }
}
// 绘制资源图标（修正：色块在方框内 + putimage 标准格式不报错）
void DiscardPanel::drawResources() {
    int currentX = RES_START_X;
    settextstyle(20, 0, _T("Arial Black"));

    // 资源配色（美观且区分明显）
    COLORREF resColors[] = { RGB(34, 139, 34), RGB(178, 34, 34), RGB(152, 251, 152), RGB(255, 215, 0), RGB(105, 105, 105) };

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        ResourceType type = (ResourceType)i;
        int ownCount = player.getResourceCount(type);
        int selectCount = selectedArray[i];

        // 统一坐标基准：色块和边框完全对齐
        int blockX = currentX - 2;       // 边框左坐标
        int blockY = RES_START_Y - 5;    // 边框上坐标
        int blockSize = ICON_SIZE + 4;   // 边框宽高（ICON_SIZE+4，和色块一致）

        // 1. 资源背景（优先显示图片，无图显示纯色块）
        if (hasImagesLoaded && resourceImages[i].getwidth() > 0) {
            // ########## 核心修正：用 EasyX 标准 putimage 格式（无缩放，避免报错）##########
            // 图片位置和色块一致，刚好在方框内（图片尺寸建议和 ICON_SIZE 一致，如100x100）
            putimage(blockX, blockY, &resourceImages[i]);
        }
        else {
            // 色块完全填充边框，无偏移
            setfillcolor(resColors[i]);
            solidrectangle(blockX, blockY, blockX + blockSize, blockY + blockSize);
        }

        // 2. 选中标记（右上角内侧，不超出边框）
        if (selectCount > 0) {
            setfillcolor(RGB(255, 0, 0));
            solidcircle(blockX + blockSize - 20, blockY + 20, 18); // 方框内右上角
            settextcolor(WHITE);
            std::string selectStr = std::to_string(selectCount);
            int sw = textwidth(_T(selectStr.c_str()));
            int sh = textheight(_T(selectStr.c_str()));
            outtextxy(blockX + blockSize - 20 - sw / 2, blockY + 20 - sh / 2, _T(selectStr.c_str()));
        }

        // 3. 拥有数量（色块下方居中）
        settextcolor(ownCount > 0 ? YELLOW : DARKGRAY);
        std::string countStr = "x " + std::to_string(ownCount);
        int nw = textwidth(_T(countStr.c_str()));
        outtextxy(blockX + (blockSize - nw) / 2, blockY + blockSize + 5, _T(countStr.c_str()));

        // 4. 可点击高亮框（包围色块，无偏移）
        if (ownCount > 0 && remaining > 0) {
            setlinecolor(WHITE);
            setlinestyle(PS_DOT, 2);
            rectangle(blockX, blockY, blockX + blockSize, blockY + blockSize);
        }

        currentX += (ICON_SIZE + ICON_GAP);
    }
}

// 绘制按钮（模拟启用/禁用，不修改Button类）
void DiscardPanel::drawButtons() {
    // 1. 绘制取消按钮（始终启用，直接画）
    cancelBtn.draw();

    // 2. 绘制确认按钮（模拟启用/禁用）
    confirmBtn.draw();
    if (!isConfirmBtnEnabled) {
        // 禁用状态：叠加半透明灰色遮罩（不修改Button类，仅覆盖绘制）
        setfillcolor(BTN_DISABLE_MASK);
        solidroundrect(confirmBtn.x, confirmBtn.y, confirmBtn.x + 120, confirmBtn.y + 50, 8, 8);
        // 叠加深色文字（强化禁用视觉）
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(DARK_TEXT);
        int tw = textwidth(_T("确认弃牌"));
        int th = textheight(_T("确认弃牌"));
        outtextxy(confirmBtn.x + (120 - tw) / 2, confirmBtn.y + (50 - th) / 2, _T("确认弃牌"));
    }
}

// 更新按钮状态（自己维护，不依赖Button类）
void DiscardPanel::updateButtonStates() {
    isConfirmBtnEnabled = (remaining == 0); // 选够弃牌数则启用
}

// 判断是否点击了资源块
bool DiscardPanel::isResourceClicked(int mouseX, int mouseY, int& resIndex) {
    int currentX = RES_START_X;
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (mouseX >= currentX && mouseX <= currentX + ICON_SIZE &&
            mouseY >= RES_START_Y && mouseY <= RES_START_Y + ICON_SIZE) {
            resIndex = i;
            return true;
        }
        currentX += (ICON_SIZE + ICON_GAP);
    }
    return false;
}

// 计算已选中资源总数
int DiscardPanel::getSelectedTotal() {
    int total = 0;
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        total += selectedArray[i];
    }
    return total;
}

// 重置选中状态（归还资源）
void DiscardPanel::resetSelected() {
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        if (selectedArray[i] > 0) {
            player.addResource((ResourceType)i, selectedArray[i]);
            selectedArray[i] = 0;
        }
    }
    remaining = requiredDiscard;
}

// 核心：显示面板+交互逻辑（完全不修改Button类）
bool DiscardPanel::show() {
    // 初始化弃牌数量
    requiredDiscard = player.getDiscardCount();
    if (requiredDiscard <= 0) return true; // 无需弃牌
    remaining = requiredDiscard;
    isConfirmBtnEnabled = false; // 初始禁用确认按钮

    ExMessage msg;
    bool exit = false;
    bool discardSuccess = false;

    BeginBatchDraw();
    while (!exit) {
        // 绘制层级（从下到上：遮罩→对话框→文字→资源→按钮）
        drawMask();
        drawDialog();
        drawTexts();
        drawResources();
        updateButtonStates(); // 更新按钮状态
        drawButtons();        // 绘制按钮（含禁用遮罩）

        FlushBatchDraw(); // 刷新显示

        // 处理鼠标交互
        msg = getmessage(EM_MOUSE);
        if (msg.message == WM_LBUTTONDOWN) {
            int resIndex = -1;

            // 1. 点击确认按钮（需启用状态才有效）
            if (confirmBtn.isClicked(msg.x, msg.y) && isConfirmBtnEnabled) {
                discardSuccess = true;
                exit = true;
            }

            // 2. 点击重新选择按钮（直接有效）
            else if (cancelBtn.isClicked(msg.x, msg.y)) {
                resetSelected();
            }

            // 3. 点击资源块（选择/取消选择）
            else if (isResourceClicked(msg.x, msg.y, resIndex)) {
                ResourceType type = (ResourceType)resIndex;
                int ownCount = player.getResourceCount(type);
                int selectCount = selectedArray[resIndex];

                if (remaining > 0 && ownCount > 0) {
                    // 未选够：选中资源
                    player.removeResource(type, 1);
                    selectedArray[resIndex]++;
                    remaining--;
                }
                else if (remaining == 0 && selectCount > 0) {
                    // 已选够：取消选中
                    player.addResource(type, 1);
                    selectedArray[resIndex]--;
                    remaining++;
                }
            }
        }
    }
    EndBatchDraw();

    // 取消弃牌：归还所有选中资源
    if (!discardSuccess) {
        resetSelected();
    }

    return discardSuccess;
}