#include "HarborPanel.h"
#include <cstdio>
#include <tchar.h>
#include <string>
#include <sstream>

// --- 资源名称定义 ---
static const TCHAR* RES_NAMES[] = {
    _T("Wood"), _T("Brick"), _T("Sheep"), _T("Wheat"), _T("Ore")
};

// --- 全局布局常量 (全屏模式) ---
static const int ICON_SIZE = 100;  // 图标大一点，适应全屏
static const int ICON_GAP = 60;    // 图标之间的间距

// Helper: map resource index -> tradeOptions index for specific 2:1
// tradeOptions layout: [4:1, 3:1, 木材2:1, 砖块2:1, 矿石2:1, 羊毛2:1, 粮食2:1]
// resourceIndex: 0=Wood,1=Brick,2=Sheep,3=Wheat,4=Ore
static int resourceIndexToTradeOptionsIdx(int resIdx) {
    // mapping based on the order provided by user:
    // tradeOptions[2] -> Wood
    // tradeOptions[3] -> Brick
    // tradeOptions[4] -> Ore
    // tradeOptions[5] -> Sheep
    // tradeOptions[6] -> Wheat
    switch (resIdx) {
    case 0: return 2; // Wood
    case 1: return 3; // Brick
    case 2: return 5; // Sheep
    case 3: return 6; // Wheat
    case 4: return 4; // Ore
    default: return -1;
    }
}

HarborPanel::HarborPanel(int screenWidth, int screenHeight, const std::vector<bool>& tradeOptions)
    : width(screenWidth), height(screenHeight),
    selectedGiveType(-1), hasImagesLoaded(false), hasBgLoaded(false),
    tradeOptions(tradeOptions)
{
    for (int i = 0; i < RESOURCE_COUNT; i++) getQuantities[i] = 0;

    // 按钮布局：放在屏幕底部居中 (保留原始魔数，和原来行为一致)
    int btnW = 200;
    int btnH = 70;
    int btnX = 740;
    int btnY = 820;

    // 确认按钮
    btnConfirm = new Button(
        btnX,
        btnY,
        btnW, btnH,
        "Confirm Trade",
        RGB(38,111,64)
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

void HarborPanel::loadResourceImages(const char* filePaths[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        loadimage(&resourceImages[i], (LPCTSTR)filePaths[i], ICON_SIZE, ICON_SIZE, true);
    }
    hasImagesLoaded = true;
}

// 【新增】加载背景图片实现
void HarborPanel::loadBackgroundImage(const char* filePath) {
    // 缩放加载图片以适应面板大小
    loadimage(&bgImage, (LPCTSTR)filePath, width, height, true);
    hasBgLoaded = true;
}

int HarborPanel::getResourceAtClick(int mouseX, int mouseY, int uiOffsetX, int uiOffsetY) {
    // 全屏模式下逻辑内嵌，此函数暂时不用，但保留以防报错
    return -1;
}

void HarborPanel::drawArtisticText(int x, int y, const TCHAR* text, int size, COLORREF mainColor) {
    setbkmode(TRANSPARENT);
    settextstyle(size, 0, _T("Arial Black"));

    // 阴影
    settextcolor(RGB(40, 40, 40));
    outtextxy(x + 3, y + 3, text);

    // 本体
    settextcolor(mainColor);
    outtextxy(x, y, text);

    // 高光
    settextcolor(RGB(111,94,86));
    outtextxy(x - 1, y - 1, text);
}

// 返回 2/3/4 表示该资源的兑换率，返回 0 表示当前不可兑换
int HarborPanel::getRateForResource(int resourceIndex) const {
    // safety: ensure tradeOptions has at least 7 elements; if not, fall back to default 4:1
    bool has4 = false, has3 = false;
    if (tradeOptions.size() >= 1) has4 = tradeOptions[0];
    if (tradeOptions.size() >= 2) has3 = tradeOptions[1];

    int specificIdx = resourceIndexToTradeOptionsIdx(resourceIndex);
    bool hasSpecific = false;
    if (specificIdx >= 0 && (size_t)specificIdx < tradeOptions.size()) {
        hasSpecific = tradeOptions[specificIdx];
    }

    if (hasSpecific) return 2;
    if (has3) return 3;
    if (has4) return 4;
    // no applicable port
    return 0;
}

void HarborPanel::draw(const Player& player, int mouseX, int mouseY) {
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
    const TCHAR* title = _T("Harbor RESOURCE EXCHANGE");
    settextstyle(50, 0, _T("Arial Black"));
    int titleW = textwidth(title);
    // remove magic offset and center based on titleW (keeps baseline left shift small)
    drawArtisticText((width - titleW) / 2, 50, title, 50);

    // 3. 计算垂直布局 Y 坐标
    int getY = height * 0.25;      // GET区在屏幕 1/4 处
    int middleY = height * 0.45;   // 汇率在中间
    int giveY = height * 0.60;     // GIVE区在屏幕 3/5 处

    // 计算总花费 (按数量)
    int totalGetCount = 0;
    for (int n : getQuantities) totalGetCount += n;

    // 4. 中心汇率显示：
    // 如果已经选中支出资源，则显示该资源对应的汇率；否则显示可用港口概览
    TCHAR rateBuf[128];
    if (selectedGiveType != -1) {
        int rate = getRateForResource(selectedGiveType);
        if (rate > 0) {
            _stprintf_s(rateBuf, _T("Exchange Rate %d : 1"), rate);
        } else {
            _stprintf_s(rateBuf, _T("Exchange Not Available for %s"), RES_NAMES[selectedGiveType]);
        }
        settextstyle(30, 0, _T("Arial Black"));
        int rateW = textwidth(rateBuf);
        drawArtisticText((width - rateW) / 2, middleY, rateBuf, 30, RGB(220, 20, 60));
    } else {
        // 显示可用港口概览
        std::basic_ostringstream<TCHAR> legend;
        legend << _T("Available Ports:");
        if (tradeOptions.size() >= 7) {
            if (tradeOptions[2]) legend << _T(" Wood(2:1)");
            if (tradeOptions[3]) legend << _T(" Brick(2:1)");
            if (tradeOptions[4]) legend << _T(" Ore(2:1)");
            if (tradeOptions[5]) legend << _T(" Sheep(2:1)");
            if (tradeOptions[6]) legend << _T(" Wheat(2:1)");
        }
        if (tradeOptions.size() >= 2 && tradeOptions[1]) legend << _T(" 3:1");
        if (tradeOptions.size() >= 1 && tradeOptions[0]) legend << _T(" 4:1");

        std::basic_string<TCHAR> legendStr = legend.str();
        settextstyle(22, 0, _T("Arial"));
        int legendW = textwidth(legendStr.c_str());
        drawArtisticText((width - legendW) / 2, middleY, legendStr.c_str(), 22, RGB(70,70,70));
    }

    // 5. 计算图标行的水平起始位置 (确保5个图标整体居中)
    int totalRowWidth = 5 * ICON_SIZE + 4 * ICON_GAP;
    int startX = (width - totalRowWidth) / 2;

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

        // 计算该资源对应的实际兑换率（2/3/4/0）
        int rateForThis = getRateForResource(i);

        // 计算当前选择的目标数总和的所需成本（依据 rateForThis）
        int costNeededForThis;
        if (totalGetCount == 0) {
            // 若没有选择任何 GET 单位，按单个单位的费率来显示是否可用/可选
            costNeededForThis = (rateForThis > 0) ? rateForThis : INT_MAX;
        } else {
            if (rateForThis > 0)
                costNeededForThis = totalGetCount * rateForThis;
            else
                costNeededForThis = INT_MAX;
        }

        bool canAfford = (playerRes >= costNeededForThis);
        bool hoverGive = (mouseX >= xPos && mouseX <= xPos + ICON_SIZE && mouseY >= giveY && mouseY <= giveY + ICON_SIZE);

        // 绘制图标
        if (hasImagesLoaded) putimage(xPos, giveY, &resourceImages[i]);
        else { setfillcolor(BLACK); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE); }

        // 状态效果
        if (rateForThis == 0) {
            // 不可兑换 -> 置灰并标示
            setfillcolor(0xAA000000); // 置灰
            solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);

            // 显示 "N/A"
            settextstyle(18, 0, _T("Arial"));
            settextcolor(RGB(150, 150, 150));
            outtextxy(xPos + (ICON_SIZE - textwidth(_T("N/A"))) / 2, giveY + ICON_SIZE / 2 - 10, _T("N/A"));
        }
        else {
            if (!canAfford) {
                // 不满足资源数量 -> 半透明灰
                setfillcolor(0xAA000000);
                solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
            } else {
                if (hoverGive) {
                    setfillcolor(0x44FFFFFF); solidrectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
                    setlinecolor(WHITE); setlinestyle(PS_SOLID, 2); rectangle(xPos, giveY, xPos + ICON_SIZE, giveY + ICON_SIZE);
                }
                if (selectedGiveType == i) {
                    setlinecolor(RGB(255, 215, 0)); setlinestyle(PS_SOLID, 5);
                    rectangle(xPos - 4, giveY - 4, xPos + ICON_SIZE + 4, giveY + ICON_SIZE + 4);
                }
            }

            // 在 GIVE 图标上角显示费率小角标 (例如 "2:1")
            TCHAR badge[16];
            _stprintf_s(badge, _T("%d:1"), rateForThis);
            settextstyle(18, 0, _T("Arial"));
            settextcolor(RGB(255, 255, 255));
            // badge 背景
            setfillcolor(RGB(80, 80, 80));
            solidrectangle(xPos + ICON_SIZE - 44, giveY + 6, xPos + ICON_SIZE - 6, giveY + 30);
            outtextxy(xPos + ICON_SIZE - 42, giveY + 8, badge);
        }

        // 文字: 数量
        TCHAR giveBuf[32];
        _stprintf_s(giveBuf, _T("%s: %d"), RES_NAMES[i], playerRes);
        settextstyle(20, 0, _T("Arial"));
        settextcolor(canAfford ? BLACK : RGB(100, 100, 100));
        outtextxy(xPos + (ICON_SIZE - textwidth(giveBuf)) / 2, giveY + ICON_SIZE + 10, giveBuf);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 按钮状态
    bool isValid = (selectedGiveType != -1 && totalGetCount > 0);
    if (isValid) {
        int rateSelected = getRateForResource(selectedGiveType);
        if (rateSelected <= 0) isValid = false;
        else {
            int cost = totalGetCount * rateSelected;
            if (player.getResourceCount((ResourceType)selectedGiveType) < cost) isValid = false;
        }
    }

    btnConfirm->enabled = isValid;
    btnConfirm->color = isValid ? RGB(255, 140, 0) : RGB(100, 100, 100);

    btnConfirm->draw();
    btnCancel->draw();
}

bool HarborPanel::handleInput(ExMessage& msg, Player& player) {
    // 先确保我们只在鼠标消息中使用 msg.x/msg.y
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
            int rate = getRateForResource(selectedGiveType);
            if (rate > 0 && totalGet > 0 && player.getResourceCount(give) >= totalGet * rate) {
                player.removeResource(give, totalGet * rate);
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
                int rate = getRateForResource(i);
                int cost = (totalGet == 0) ? (rate > 0 ? rate : INT_MAX) : (rate > 0 ? totalGet * rate : INT_MAX);
                if (rate > 0 && player.getResourceCount((ResourceType)i) >= cost) selectedGiveType = i;
            }
            else if (msg.message == WM_RBUTTONDOWN && selectedGiveType == i) {
                selectedGiveType = -1;
            }
        }
    }
    return false;
}