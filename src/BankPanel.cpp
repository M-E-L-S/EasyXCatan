#include "BankPanel.h"
#include <cstdio>
#include <tchar.h>
#include <string>

// --- 资源名称定义 ---
static const TCHAR* RES_NAMES[] = {
    _T("Wood"), _T("Brick"), _T("Sheep"), _T("Wheat"), _T("Ore")
};

// --- 全局布局常量 (全屏模式) ---
static const int ICON_SIZE = 100;  // 图标大一点，适应全屏
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

    // 确认按钮
    btnConfirm = new Button(
        btnX,
        btnY,
        btnW, btnH,
        "Confirm Trade",
        RGB(100, 100, 100)
    );
    btnConfirm->enabled = false;

    // 关闭按钮：放在屏幕右上角
    btnCancel = new Button(
        width - 60,
        20,
        40, 40,
        "X",
        RGB(200, 60, 60)
    );
}

void BankPanel::loadResourceImages(const char* filePaths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        loadimage(&resourceImages[i], (LPCTSTR)filePaths[i], ICON_SIZE, ICON_SIZE, true);
    }
    hasImagesLoaded = true;
}

// 【新增】加载背景图片实现
void BankPanel::loadBackgroundImage(const char* filePath) {
    // 缩放加载图片以适应面板大小
    loadimage(&bgImage, (LPCTSTR)filePath, width, height, true);
    hasBgLoaded = true;
}

int BankPanel::getResourceAtClick(int mouseX, int mouseY, int uiOffsetX, int uiOffsetY) {
    // 全屏模式下逻辑内嵌，此函数暂时不用，但保留以防报错
    return -1;
}

void BankPanel::drawArtisticText(int x, int y, const TCHAR* text, int size, COLORREF mainColor) {
    setbkmode(TRANSPARENT);
    settextstyle(size, 0, _T("Arial Black"));

    // 阴影
    settextcolor(RGB(40, 40, 40));
    outtextxy(x + 3, y + 3, text);

    // 本体
    settextcolor(mainColor);
    outtextxy(x, y, text);

    // 高光
    settextcolor(WHITE);
    outtextxy(x - 1, y - 1, text);
}

void BankPanel::draw(const Player& player, int mouseX, int mouseY) {
    // 1. 绘制全屏背景
    if (hasBgLoaded) {
        putimage(0, 0, &bgImage); // 贴背景图
    }
    else {
        // 兜底：全屏羊皮纸色
        setfillcolor(RGB(245, 222, 179));
        solidrectangle(0, 0, width, height);
        // 画一个全屏边框装饰
        setlinecolor(RGB(101, 67, 33));
        setlinestyle(PS_SOLID, 10);
        rectangle(0, 0, width, height);
    }

    // 2. 标题 (屏幕顶部居中)
    const TCHAR* title = _T("BANK RESOURCE EXCHANGE");
    settextstyle(50, 0, _T("Arial Black"));
    int titleW = textwidth(title);
    drawArtisticText((width - titleW) / 2, 50, title, 50);

    // 3. 计算垂直布局 Y 坐标
    int getY = height * 0.25;      // GET区在屏幕 1/4 处
    int middleY = height * 0.45;   // 汇率在中间
    int giveY = height * 0.60;     // GIVE区在屏幕 3/5 处

    // 4. 绘制中间汇率 (屏幕居中)
    const TCHAR* rateText = _T("Exchange Rate 4 : 1");
    settextstyle(30, 0, _T("Arial Black"));
    int rateW = textwidth(rateText);
    drawArtisticText((width - rateW) / 2, middleY, rateText, 30, RGB(220, 20, 60));

    // 5. 计算图标行的水平起始位置 (确保5个图标整体居中)
    int totalRowWidth = 5 * ICON_SIZE + 4 * ICON_GAP;
    int startX = (width - totalRowWidth) / 2;

    // 计算总花费
    int totalGetCount = 0;
    for (int n : getQuantities) totalGetCount += n;
    int costNeeded = (totalGetCount == 0) ? 4 : (totalGetCount * 4);

    // --- 循环绘制资源 ---
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        int xPos = startX + i * (ICON_SIZE + ICON_GAP);

        // ================= GET 区 (上方) =================
        bool hoverGet = (mouseX >= xPos && mouseX <= xPos + ICON_SIZE && mouseY >= getY && mouseY <= getY + ICON_SIZE);

        // 绘制图标
        if (hasImagesLoaded) putimage(xPos, getY, &resourceImages[i]);
        else { setfillcolor(BLACK); solidrectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE); }

        // 高亮/选中效果
        if (hoverGet) {
            setfillcolor(0x44FFFFFF); solidrectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE);
            setlinecolor(WHITE); setlinestyle(PS_SOLID, 2); rectangle(xPos, getY, xPos + ICON_SIZE, getY + ICON_SIZE);
        }
        if (getQuantities[i] > 0) {
            setlinecolor(GREEN); setlinestyle(PS_SOLID, 4);
            rectangle(xPos - 4, getY - 4, xPos + ICON_SIZE + 4, getY + ICON_SIZE + 4);
        }

        // 文字
        TCHAR getBuf[32];
        if (getQuantities[i] > 0) _stprintf_s(getBuf, _T("%s +%d"), RES_NAMES[i], getQuantities[i]);
        else _stprintf_s(getBuf, _T("%s"), RES_NAMES[i]);

        settextstyle(20, 0, _T("Arial Bold"));
        settextcolor(getQuantities[i] > 0 ? RGB(0, 128, 0) : RGB(50, 50, 50));
        outtextxy(xPos + (ICON_SIZE - textwidth(getBuf)) / 2, getY + ICON_SIZE + 10, getBuf);


        // ================= GIVE 区 (下方) =================
        int playerRes = player.getResourceCount((ResourceType)i);
        bool canAfford = (playerRes >= costNeeded);
        bool hoverGive = (mouseX >= xPos && mouseX <= xPos + ICON_SIZE && mouseY >= giveY && mouseY <= giveY + ICON_SIZE);

        // 绘制图标
        if (hasImagesLoaded) putimage(xPos, giveY, &resourceImages[i]);
        else { setfillcolor(BLACK); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE); }

        // 状态效果
        if (!canAfford) {
            setfillcolor(0xAA000000); // 置灰
            solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
        }
        else {
            if (hoverGive) {
                setfillcolor(0x44FFFFFF); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
                setlinecolor(WHITE); setlinestyle(PS_SOLID, 2); rectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
            }
            if (selectedGiveType == i) {
                setlinecolor(RGB(255, 215, 0)); setlinestyle(PS_SOLID, 5);
                rectangle(xPos - 4, giveY - 4, xPos + ICON_SIZE + 4, giveY + ICON_SIZE + 4);
            }
        }

        // 文字
        TCHAR giveBuf[32];
        _stprintf_s(giveBuf, _T("%s: %d"), RES_NAMES[i], playerRes);
        settextstyle(20, 0, _T("Arial"));
        settextcolor(canAfford ? BLACK : RGB(100, 100, 100));
        outtextxy(xPos + (ICON_SIZE - textwidth(giveBuf)) / 2, giveY + ICON_SIZE + 10, giveBuf);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 按钮状态
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
            // 执行交易
            ResourceType give = (ResourceType)selectedGiveType;
            int totalGet = 0;
            for (int n : getQuantities) totalGet += n;
            if (player.getResourceCount(give) >= totalGet * 4) {
                player.removeResource(give, totalGet * 4);
                for (int i = 0; i < RESOURCE_COUNT; i++)
                    if (getQuantities[i] > 0) player.addResource((ResourceType)i, getQuantities[i]);
                // 重置
                selectedGiveType = -1;
                for (int i = 0; i < RESOURCE_COUNT; i++) getQuantities[i] = 0;
            }
            return false;
        }
    }

    // 图标交互计算 (这里的逻辑和 draw 一致)
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