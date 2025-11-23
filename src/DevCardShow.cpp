#include "DevCardShow.h"
#include <graphics.h>
#include <conio.h>
#include <windows.h>

int ShowDevCardPanel(Player& player, DevCardManager& manager) {
DevCardPanel panel(10, 30);  // 创建发展卡面板
panel.setVisible(true);
panel.update(player, manager);

```
while (true) {
    cleardevice();
    panel.draw(manager, player);

    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {
        if (msg.message == WM_LBUTTONDOWN) {
            int result = panel.handleClick(msg.x, msg.y);
            if (result >= 0 && result < DEV_CARD_COUNT) {
                // 玩家选择的卡牌类型
                return result;
            }
        }
    }

    // 如果按 ESC，返回 -1
    if (_kbhit() && _getch() == 27)
        return -1;

    Sleep(10);
    FlushBatchDraw();
}
```

}
