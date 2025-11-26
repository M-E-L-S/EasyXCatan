#include <graphics.h>
#include <conio.h>
#include<windows.h>
#include<mmsystem.h>
#include <cstdio>
#include<string>
#include "map.h"
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int main()
{
    initgraph(screenWidth, screenHeight,EX_SHOWCONSOLE);
   IMAGE A,B;
    setbkcolor(BLUE);
    cleardevice();
    loadimage(&A,"background.jpg",screenWidth,screenHeight);
    putimage( 0,0,&A);
    Map map(0);
    map.initMap(A);
    map.drawAll();
    map.buildVillage(3,1);
    map.drawAllBuildings();
    map.moveRobber(0, 10);
    // 移动强盗     //1
    getchar();
    return 0;
}
