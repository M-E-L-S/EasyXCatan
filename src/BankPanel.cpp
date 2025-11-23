#include "BankPanel.h"
#include <cstdio>
#include <tchar.h>
#include <string>

// --- 资源名称定义 ---
static const TCHAR* RES_NAMES[] = {
    _T("Wood"), _T("Brick"), _T("Sheep"), _T("Wheat"), _T("Ore")
};

// --- 全局布局常量 (全屏模式) ---
static const int ICON_SIZE = 100;  // 图标改大一点，适应全屏
static const int ICON_GAP = 60;    // 图标之间的间距

BankPanel::BankPanel(int screenWidth, int screenHeight)
    : width(screenWidth), height(screenHeight),
    selectedGiveType(-1), hasImagesLoaded(false), hasBgLoaded(false)
{
    for (int i = 0; i < RESOURCE_COUNT; i++) getQuantities[i] = 0;

    // 按钮布局：放在屏幕底部居中
    int btnW = 250;
    int btnH = 60;
    int btnX = (width - btnW) / 2;
    int btnY = height - 100; // 距离底部 100 像素

    btnConfirm = new Button(btnX, btnY, btnW, btnH, "Confirm Trade", RGB(100, 100, 100));
    btnConfirm->enabled = false;

    // 关闭按钮：放在屏幕右上角
    btnCancel = new Button(width - 60, 20, 40, 40, "X", RGB(200, 60, 60));
}

void BankPanel::loadResourceImages(const char* filePaths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        // 加载时缩放到 ICON_SIZE 大小
        loadimage(&resourceImages[i], (LPCTSTR)filePaths[i], ICON_SIZE, ICON_SIZE, true);
    }
    hasImagesLoaded = true;
}

void BankPanel::loadBackgroundImage(const char* filePath) {
    // 背景图拉伸至全屏
    loadimage(&bgImage, (LPCTSTR)filePath, width, height, true);
    hasBgLoaded = true;
}

int BankPanel::getResourceAtClick(int mouseX, int mouseY, int uiOffsetX, int uiOffsetY) {
    // 全屏模式下不需要这个旧的辅助函数了，逻辑已内嵌到 handleInput
    return -1;
}

void BankPanel::drawArtisticText(int x, int y, const TCHAR* text, int size, COLORREF mainColor) {
    setbkmode(TRANSPARENT);
    settextstyle(size, 0, _T("Arial Black"));
    // 阴影
    settextcolor(RGB(40, 40, 40)); outtextxy(x + 4, y + 4, text);
    // 本体
    settextcolor(mainColor);       outtextxy(x, y, text);
    // 高光
    settextcolor(WHITE);           outtextxy(x - 1, y - 1, text);
}

void BankPanel::draw(const Player& player, int mouseX, int mouseY) {
    // 1. 绘制全屏背景
    if (hasBgLoaded) {
        putimage(0, 0, &bgImage); // 贴背景图
    }
    else {
        // 兜底：如果没有背景图，用羊皮纸色填充全屏
        setfillcolor(RGB(245, 222, 179));
        solidrectangle(0, 0, width, height);
        // 画一个全屏装饰边框
        setlinecolor(RGB(101, 67, 33));
        setlinestyle(PS_SOLID, 10);
        rectangle(0, 0, width, height);
    }

    // 2. 顶部标题 (水平居中)
    const TCHAR* title = _T("BANK RESOURCE EXCHANGE");
    settextstyle(50, 0, _T("Arial Black"));
    int titleW = textwidth(title);
    drawArtisticText((width - titleW) / 2, 40, title, 50);

    settextstyle(25, 0, _T("Arial"));
    settextcolor(RGB(80, 80, 80));
    const TCHAR* subtitle = _T("Select items to trade");
    outtextxy((width - textwidth(subtitle)) / 2, 100, subtitle);

    // --- 计算动态布局坐标 ---
    // 垂直位置：基于屏幕高度百分比
    int getY = height * 0.25;      // GET区在 25% 高度
    int middleY = height * 0.48;   // 汇率在 48% 高度
    int giveY = height * 0.60;     // GIVE区在 60% 高度

    // 水平位置：计算 5 个图标的总宽度，然后居中
    int totalRowWidth = 5 * ICON_SIZE + 4 * ICON_GAP;
    int startX = (width - totalRowWidth) / 2;

    // 3. 绘制中间汇率
    const TCHAR* rateText = _T("Exchange Rate  4 : 1");
    settextstyle(35, 0, _T("Arial Black"));
    int rateW = textwidth(rateText);
    drawArtisticText((width - rateW) / 2, middleY, rateText, 35, RGB(220, 20, 60));

    // 4. 计算交易成本
    int totalGetCount = 0;
    for (int n : getQuantities) totalGetCount += n;
    int costNeeded = (totalGetCount == 0) ? 4 : (totalGetCount * 4);

    // 5. 循环绘制资源
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        int xPos = startX + i * (ICON_SIZE + ICON_GAP);

        // ===========================
        //        GET 区 (上方)
        // ===========================
        bool hoverGet = (mouseX >= xPos && mouseX <= xPos + ICON_SIZE && mouseY >= getY && mouseY <= getY + ICON_SIZE);

        // 图标
        if (hasImagesLoaded) putimage(xPos, getY, &resourceImages[i]);
        else { setfillcolor(BLACK); solidrectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE); }

        // 悬停高亮
        if (hoverGet) {
            setfillcolor(0x44FFFFFF); solidrectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE);
            setlinecolor(WHITE); setlinestyle(PS_SOLID, 2); rectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE);
        }
        // 选中绿框
        if (getQuantities[i] > 0) {
            setlinecolor(GREEN); setlinestyle(PS_SOLID, 5);
            rectangle(xPos - 4, getY - 4, xPos + ICON_SIZE + 4, getY + ICON_SIZE + 4);
        }

        // 文字
        TCHAR getBuf[32];
        if (getQuantities[i] > 0) _stprintf_s(getBuf, _T("%s +%d"), RES_NAMES[i], getQuantities[i]);
        else _stprintf_s(getBuf, _T("%s"), RES_NAMES[i]);

        settextstyle(22, 0, _T("Arial Bold"));
        settextcolor(getQuantities[i] > 0 ? RGB(0, 100, 0) : RGB(80, 80, 80));
        outtextxy(xPos + (ICON_SIZE - textwidth(getBuf)) / 2, getY + ICON_SIZE + 10, getBuf);


        // ===========================
        //       GIVE 区 (下方)
        // ===========================
        int playerRes = player.getResourceCount((ResourceType)i);
        bool canAfford = (playerRes >= costNeeded);
        bool hoverGive = (mouseX >= xPos && mouseX <= xPos + ICON_SIZE && mouseY >= giveY && mouseY <= giveY + ICON_SIZE);

        // 图标
        if (hasImagesLoaded) putimage(xPos, giveY, &resourceImages[i]);
        else { setfillcolor(BLACK); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE); }

        // 状态处理
        if (!canAfford) {
            // 置灰
            setfillcolor(0xAA000000);
            solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
        }
        else {
            // 悬停高亮
            if (hoverGive) {
                setfillcolor(0x44FFFFFF); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
                setlinecolor(WHITE); setlinestyle(PS_SOLID, 2); rectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
            }
            // 选中金框
            if (selectedGiveType == i) {
                setlinecolor(RGB(255, 215, 0)); setlinestyle(PS_SOLID, 6);
                rectangle(xPos - 4, giveY - 4, xPos + ICON_SIZE + 4, giveY + ICON_SIZE + 4);
            }
        }

        // 文字
        TCHAR giveBuf[32];
        _stprintf_s(giveBuf, _T("%s: %d"), RES_NAMES[i], playerRes);
        settextstyle(22, 0, _T("Arial"));
        settextcolor(canAfford ? BLACK : RGB(128, 128, 128));
        outtextxy(xPos + (ICON_SIZE - textwidth(giveBuf)) / 2, giveY + ICON_SIZE + 10, giveBuf);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 更新按钮
    bool isValid = (selectedGiveType != -1 && totalGetCount > 0);
    if (isValid && player.getResourceCount((ResourceType)selectedGiveType) < totalGetCount * 4) isValid = false;

    btnConfirm->enabled = isValid;
    btnConfirm->color = isValid ? RGB(255, 140, 0) : RGB(100, 100, 100);

    btnConfirm->draw();
    btnCancel->draw();
}

bool BankPanel::handleInput(ExMessage& msg, Player& player) {
    int mouseX = msg.x;
    int mouseY = msg.y;

    // 按钮交互
    if (msg.message == WM_LBUTTONDOWN) {
        if (btnCancel->isClicked(mouseX, mouseY)) return true;
        if (btnConfirm->isClicked(mouseX, mouseY)) {
            ResourceType give = (ResourceType)selectedGiveType;
            int totalGet = 0;
            for (int n : getQuantities) totalGet += n;

            if (player.getResourceCount(give) >= totalGet * 4) {
                player.removeResource(give, totalGet * 4);
                for (int i = 0; i < RESOURCE_COUNT; i++) {
                    if (getQuantities[i] > 0) player.addResource((ResourceType)i, getQuantities[i]);
                }
                selectedGiveType = -1;
                for (int i = 0; i < RESOURCE_COUNT; i++) getQuantities[i] = 0;
            }
            return false;
        }
    }

    // --- 重新计算坐标 (必须与 draw 一致) ---
    int getY = height * 0.25;
    int giveY = height * 0.60;
    int totalRowWidth = 5 * ICON_SIZE + 4 * ICON_GAP;
    int startX = (width - totalRowWidth) / 2;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        int x = startX + i * (ICON_SIZE + ICON_GAP);

        // GET 区 (左键+, 右键-)
        if (mouseX >= x && mouseX <= x + ICON_SIZE && mouseY >= getY && mouseY <= getY + ICON_SIZE) {
            if (msg.message == WM_LBUTTONDOWN) getQuantities[i]++;
            else if (msg.message == WM_RBUTTONDOWN && getQuantities[i] > 0) getQuantities[i]--;
        }

        // GIVE 区 (左键选, 右键取消)
        if (mouseX >= x && mouseX <= x + ICON_SIZE && mouseY >= giveY && mouseY <= giveY + ICON_SIZE) {
            if (msg.message == WM_LBUTTONDOWN) {
                int totalGet = 0; for (int n : getQuantities) totalGet += n;
                int cost = (totalGet == 0) ? 4 : (totalGet * 4);
                if (player.getResourceCount((ResourceType)i) >= cost) selectedGiveType = i;
            }
            else if (msg.message == WM_RBUTTONDOWN && selectedGiveType == i) {
                selectedGiveType = -1;
            }
        }
    }
    return false;
}