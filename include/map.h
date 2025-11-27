//
// Created by 20211 on 25-11-21.
//
#ifndef CATAN_MAP_H
#define CATAN_MAP_H

#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>
#include<iostream>
#include<time.h>

#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib,"winmm.lib")
#define CR 22  //圆半径
#define RTL 50   //矩形长
#define RTW 30   //矩形宽
#define RTR 5  //圆角矩形圆半径
#define Size 120 //六边形边长
#define PI 3.14159265358979323846
#define VILLAGEWIDTH 60
#define VILLAGEHEIGHT  60
#define CITYWIDTH 180
#define CITYHEIGHT 180
#define PICTUREWIDTH 205
#define PICTUREHEIGHT  234
#define ROBBERWIDTH 150
#define ROBBERHEIGHT 195
#define HARBOURSIZE 100
#define IMAGE_PATH "resources/image/"
#define SOUND_PATH "resources/sound/"
inline void putimage(int x,int y,IMAGE* img){
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL),x,y,w,h, GetImageHDC(img),0,0,w,h,{AC_SRC_OVER,0,255,AC_SRC_ALPHA});
}
enum ResourceType{
    r1,    //forest
    r2,    //hill
    r3,    //mountain
    r4,    //pasture
    r5,    //field
    r6     //desert
};
enum BuildingType{
    road,
    village,
    city
};
enum BuildMode {
    MODE_NONE,      // 无模式
    MODE_ROAD,      // 道路模式
    MODE_SETTLEMENT,// 建筑村庄模式
    MODE_CITY       // 建筑城市模式
};
//地形名称


//骰子点数
const int DICE_NUMBERS[] = {2, 3, 4, 5, 6, 8, 9, 10, 11, 12};
// 每个数字对应的占比
const int DICE_PROBABILITIES[] = {1, 2, 3, 4, 5, 5, 4, 3, 2, 1};

struct MapPiece{
int x,y;  //六边形 中心 坐标
ResourceType r;   //资源类型
int length;   //六边形外接圆半径
int num;   //板块数字标记（2-12）
bool hasrobber;   //是否有强盗居住
bool isHighlighted; //为true时高亮中心数字
};
extern int screenWidth ;
extern int screenHeight ;

struct Vertex{
    int x,y;
    std::vector<int> neighbourPieces;    //与该顶点直接相连的板块标记
    std::vector<int> neighbourEdges;    //与该顶点直接相连的边标记
    std::vector<int> neighbourVertices;    //与该顶点直接相连的顶点标记
    int buildingType;    //0--无 1--村庄 2--城市
    int owner;   //所属玩家的ID(无人占用为-1)
    bool isHighlighted; //是否高亮处于可点击状态
    bool isHarbour;   //判断是不是港口
};
struct myPOINT{
    double x;
    double y;
};
//边
struct Edge{
    int x1,y1,x2,y2;  //两端点坐标
    std::vector<int> neighbourPieces;   //与该边直接相连的板块标记
    int owner;   //所属玩家ID
    bool isHighlighted; //是否高亮处于可点击状态
    int isHarbour;   //港口类型为 -1（不是港口） 0（3：1） 1-5对应五种2:1专用港口
};

//按钮
struct Button {
    int x, y, width, height;
    const char* text;
    bool isActive;   //是否处于可点击状态
    BuildMode mode;
};

//地图结构
class Map{
       IMAGE bg;
       MapPiece pieces[19];   //构建板块
       Vertex vertices[60];   //储存顶点（用于建设村庄/城市）
       Edge edges[80];   //储存边（用于建设道路）
//地形总数储存
        int fromPiece;
        int toPiece;
       ResourceType resourcetype[19]={
        r1,r1,r1,r1,
        r2,r2,r2,
        r3,r3,r3,
        r4,r4,r4,r4,
        r5,r5,r5,r5,
        r6
       };
       IMAGE pieceImage[6];
       IMAGE dragonImage[4];
       IMAGE harbourImage[6];
       IMAGE villageImage[4];
       IMAGE cityImage[4];
       bool isInitialized;
       BuildMode currentMode;     // 当前建造模式

       Button buildButtons[4];    // 四个建造按钮
       unsigned int randomSeed;   // 随机数种子
       std::mt19937 rng;          // 随机数生成器

public:
      int currentPlayer;         // 当前玩家
       //地图构建相关
       explicit Map(unsigned int seed = 0);
       void set_bg(IMAGE A);
       void initMap(IMAGE A);   //初始化地图    //1
                         //标准地图备份
       void init_player();   //初始边和点的owner为-1  //1
       void loadImage();
       void shuffleResources();   //随机分布资源     //1
       void setNumbers();    //设置数字编号      //1
       void set_map();     //建立19块板块      //1
       void set_harbour();  //建立港口     //1
       void drawHarbour();   //绘制港口
       void drawMap();    //绘制地图   //1
       void drawAllRoads();   //绘制所有道路   //1
       void drawAllBuildings(); //绘制所有建筑    //1
       void drawAllRobbers();   //绘制强盗    //1
       void drawHighlights();   //绘制所有高亮的边   //1
       void drawAll();   //绘制所有    //1
       //绘制一个板块
       void drawPieces(int x, int y, int size, ResourceType type, int number = 0, bool hasRobber = false);
       void set_button();    //设置按钮   //1
       void set_circle(int x,int y,COLORREF color);   //画圆(顶点)  //1
       void set_line(int x1,int y1,int x2,int y2,COLORREF color);    //设置粗直线(边)  //1
       void drawButtons(int x,int y,int index);   //绘制按钮  //1
       void drawSettlement(int x, int y, int playerId);   // 绘制村庄 //1
       void drawCity(int x, int y, int playerId);         // 绘制城市  //1
       void drawNumberCircle(int x,int y,int num);   //绘制板块中心的带编号的圆圈  //1
       void drawRobber(int x,int y);    //绘制强盗（沙漠地区）   //1
       void drawRoad(int x,int y,int playId);
       int dfs(int player,int start,std::vector<bool>& visited);  //1
       int longRoadOwner();  //返回拥有最长道路玩家ID，若无人道路长度大于等于5，返回-1;//1

       // 随机数相关
       //void setRandomSeed(unsigned int seed); // 设置随机数种子(无需实现)
       unsigned int getRandomSeed() const { return randomSeed; } // 获取当前种子   //1
     //  void reshuffleWithNewSeed(unsigned int newSeed); // 用新种子重新洗牌    //1

       // 按钮和交互功能
       bool checkButtonClick(int x, int y);    // 检查按钮点击  //1
       void setBuildMode(BuildMode mode);      // 设置建造模式  //1
       void updateHighlights();                // 更新高亮显示  //1

       //游戏建造相关
       bool buildVillage(int vertexIndex,int playerId);   //开局直接建设村庄
       void placeRobber(int pieceIndex);                       // 放置强盗     //1
       bool moveRobber();            // 移动强盗     //1
       void handleRobberMove();   //放置强盗前高亮可放置的板块数字  //1
       std::pair<bool,std::vector<int>> handleRobberMove(int mousex,int mousey);  //转移强盗，返回周围玩家ID   //1
       bool canBuildSettlement(int vertexIndex, int playerId); // 判断是否可以建造村庄    //1
       bool canBuildCity(int vertexIndex, int playerId);       // 判断是否可以建造城市    //1
       bool canBuildRoad(int edgeIndex, int playerId);         // 判断是否可以建造道路    //1
       bool buildSettlement(int vertexIndex, int playerId);    // 建造村庄       //1
       bool buildCity(int vertexIndex, int playerId);          // 建造城市       //1
       bool buildRoad(int edgeIndex, int playerId);            // 建造道路       //1
       void handleBuildRequest(BuildingType type,int playerId,bool isBeginning = false);   //更新建时的高亮  //1
       bool handleBuildRequest(BuildingType type,int playerId,int mousex,int mousey,bool isBeginning = false);  //实现建造  //1

       //资源分配相关函数
       std::vector<std::vector<std::pair<ResourceType, int>>> distributeResources(int diceResult); //返回玩家获得资源种类及数量
       void calculateResourcesFromBuildings(int playerId, int resourceCounts[6]);  //计算玩家拿取资源数量
       std::vector<std::pair<ResourceType, int>> getPlayerResources(int playerId);  //计算玩家获得资源种类及数量

       // 获取地图信息(均已经直接给出实现)
       MapPiece* getPieces() { return pieces; }
       Vertex* getVertices() { return vertices; }
       Edge* getEdges() { return edges; }
       int getPieceCount() const { return 19; }
       int getVertexCount() const { return 54; }
       int getEdgeCount() const { return 72; }
       BuildMode getCurrentMode() const { return currentMode; }
       int getCurrentPlayer() const { return currentPlayer; }
       void setCurrentPlayer(int player) { currentPlayer = player; }

       // 根据坐标查找对应（板块/顶点/边）
       int findPieceAt(int x, int y);    //1
       int findVertexAt(int x, int y);   //1
       double pointToLineDistance(int px, int py, int x1, int y1, int x2, int y2);  //辅助函数：计算点到直线距离 //1
       bool isPointBetweenEndpoints(int px, int py, int x1, int y1, int x2, int y2, int extension);
       //判断是否在线段边界框内 //1
       int findEdgeAt(int x, int y);     //1

       //港口相关
       std::vector<bool> GetTradeOption(int playerId);   //返回玩家是否有对应权限，数组大小为7   //1
       bool hasSpecialPortForResource(int playerId, ResourceType resourceType);    //检查玩家是否有对于某资源的2：1港口  //1
       bool hasGeneralPort(int playerId);   //检查玩家是否有3：1通用港口   //1
       std::vector<int> getOuterEdges(); //找出最外层边   //1

       // 工具函数
       void calculateVerticesAndEdges();                           // 计算顶点和边的坐标，并完成顶点和边的逻辑构建（不绘图） //1
       std::vector<int> getPiecesToVertex(int vertexIndex);        // 获取顶点相邻的地块   //1
       std::vector<int> getPiecesToEdge(int edgeIndex);            // 获取边相邻的地块   //1
       std::vector<int> getVerticesToVertex(int vertexIndex);      // 获取相邻顶点      //1
       std::vector<int> getEdgesToVertex(int vertexIndex);         // 获取相邻边       //1
       std::vector<int> getEdgesToEdges(int edgeIndex);            //获取边的所以相邻边  //1
       std::vector<int> getBuildableVertices(int playerId);        // 获取可建造的顶点   //1
       std::vector<int> getBuildableEdges(int playerId);           // 获取可建造的边    //1

       //建立关系
       void establishVertexNeighbours();  // 建立顶点之间的相邻关系   //1
       void establishEdgeVertexRelations();  //建立顶点与边之间的相邻关系   //1

private:
       void drawHexagonBorder(int x, int y, int size);          // 绘制六边形边框,size指外接圆半径
       COLORREF getResourceColor(ResourceType type);            // 获取资源颜色
       const char* getResourceName(ResourceType type);          // 获取资源名称
       COLORREF getNumberColor(int number);                     // 获取数字颜色（根据概率）
       bool isValidNumberPlacement(int pieceIndex, int number); // 检查数字放置是否有效
       int findDesertPiece();                                   // 找到沙漠地块
       void drawVertex(int vertexIndex);                        // 绘制顶点
       void drawEdge(int edgeIndex);                            // 绘制边
       void clearHighlights();                                  // 清除所有高亮

       // 随机数工具函数
       int randomInt(int min, int max);                // 生成区间随机整数
       double randomDouble(double min, double max);    // 生成区间随机浮点数

};
#endif //CATAN_MAP_H
