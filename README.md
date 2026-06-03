# 南苏岛

南苏岛（又名EasyXCatan）是一个基于 **C++ & EasyX** 的轻量级桌面游戏项目，围绕经典桌面游戏《卡坦岛》式的资源、建造、港口交易、发展卡与强盗机制展开。

本项目是一个以本地图形界面为核心的本地4人对战游戏，拥有地图和个人双交互界面，并已实现全部局内逻辑。

## 玩法简介

### CATAN（卡坦岛）

CATAN（原名 *The Settlers of CATAN*）是一款经典的资源管理与交易桌游。玩家在随机生成的岛屿上通过掷骰获取木材、砖块、羊毛、谷物和矿石等资源，进而建设道路、定居点与城市，并通过交易与扩张争取率先达到 10 分获胜。

### 官方规则
- [CATAN 官方规则页](https://www.catan.com/understand-catan/game-rules)
- [基础版规则书 PDF](https://www.catan.com/sites/default/files/2021-06/catan_base_rules_2020_200707.pdf)

## 项目背景
在我们决定制作平面桌游后，具体的参考游戏蓝本倒是简单了，选择了最为熟知、最为流行的欧洲中型桌游《卡坦岛》。风格化的六边形地图，经典的资源采集玩法，有丰富拓展性的附加模块功能卡和玩家交易，以及成就系统，既一目了然也便于我们在此基础上做出更改。

于是我们先完全仿制了一模一样的规则，之后在底层世界观上开始重构游戏环境。切合实际地把场景改为了南苏校园，重新设计了功能卡，新增了骰子特殊事件，解决了原版游戏由于骰子概率分布差异大导致的资源单一以及中期流程冗长问题，加速了游戏节奏，缓解了拥王者效应，也算是给古老桌游重焕生机吧。


## 运行环境

- C++20
- **构建工具**：CMake
- **图形库**：[EasyX for mingw](https://codebus.cn/bestans/easyx-for-mingw)
- **平台依赖**：Windows
- **额外链接**：`winmm`、`MSIMG32`

## 代码结构

```text
EasyXCatan/
├── src/          # 源码
├── include/      # 头文件
├── assets/       # 玩家界面图片、背景、音效等资源
├── resources/    # 地图界面资源
├── CMakeLists.txt
└── README.md
```

## 构建说明

项目使用 CMake 构建，核心目标会链接 EasyX 及其相关依赖库。由于工程依赖 Windows 与 EasyX，在编译前请确认操作系统和EasyX安装。

```bash
cmake -S . -B build
cmake --build build
```

## 致谢

感谢参与 EasyXCatan 相关开发、测试和调试的组员。

感谢桌游 CATAN（卡坦岛）提供的框架蓝本。