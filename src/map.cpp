//
// Created by 20211 on 25-11-21.
//

#include "map.h"

void Map::set_circle(int x, int y,COLORREF color) {
    setfillcolor(color);
    solidcircle(x,y,CR);
}
void Map::set_line(int x1,int y1,int x2,int y2,COLORREF color){
    setlinecolor(color);
    line(x1,y1,x2,y2);
}
void Map::drawButtons(int x,int y,int index) {
    int left = x - RTL/2 ;
    int right = x + RTL/2 ;
    int top = y - RTW/2;
    int bottom = y + RTW/2;
    setfillcolor(RED);
    fillroundrect(left,top,right,bottom,RTR,RTR);
    const char* s1=buildButtons[0].text;
    const char* s2=buildButtons[1].text;
    const char* s3=buildButtons[2].text;
    const char* s4=buildButtons[3].text;
    switch(index){
        case 0:
            outtextxy(x-textwidth(s1)/2,y- textheight(s1)/2,s1);
            break;
        case 1:
            outtextxy(x-textwidth(s2)/2,y- textheight(s2)/2,s2);
            break;
        case 2:
            outtextxy(x-textwidth(s3)/2,y- textheight(s3)/2,s3);
            break;
        case 3:
            outtextxy(x-textwidth(s4)/2,y- textheight(s4)/2,s4);
            break;
        default:
            break;

    }
}
void Map::drawNumberCircle(int x,int y,int num){
    if(num == -1 ) return;
    setfillcolor(BLUE);
    solidcircle(x,y,CR);
    char s[4]="";
    sprintf_s(s,"%d",num);
  //  printf("there is %d\n",num);
    settextstyle(30,17,"Consolas");
    outtextxy(x- textwidth(s)/2,y- textheight(s)/2,s);
}
void Map::drawSettlement(int x, int y, int playerId) {
    putimage(x-VILLAGEWIDTH/2,y-VILLAGEHEIGHT/2,&villageImage[playerId-1]);
}
void Map::drawCity(int x, int y, int playerId) {

    putimage(x-CITYWIDTH/2,y-CITYHEIGHT,&cityImage[playerId-1]);

}
void Map::drawRobber(int x,int y) {
    putimage(x-ROBBERWIDTH/2,y-ROBBERHEIGHT/2,&dragonImage[0]);
}
int Map::findPieceAt(int x, int y) {
    const int mouseTolerance = Size * sqrt(3)/2; //鼠标点击距离板块正中心的误差范围
    for (int i = 0; i < 19; i++) {
        MapPiece piece = pieces[i];
        int dx = x - piece.x;
        int dy = y - piece.y;
        double distance = sqrt(dx * dx + dy * dy);
        if (distance < mouseTolerance) {
            return i;
        }
    }
    return -1;   //未找到
}
int Map::findVertexAt(int x, int y){
    for (int i = 0; i < 54; i++) {
        Vertex vertex = vertices[i];
        int dx = x - vertex.x;
        int dy = y - vertex.y;
        double distance = sqrt(dx * dx + dy * dy);
        if (distance < CR) {
            return i;
        }
    }
    return -1; // 未找到
}
double Map::pointToLineDistance(int px, int py, int x1, int y1, int x2, int y2) {
    double lineLength = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    if (lineLength == 0) return sqrt(pow(px - x1, 2) + pow(py - y1, 2));
    // 计算投影参数
    double t = std::max(0.0,std::min(1.0,((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / (lineLength * lineLength)));
    // 计算投影点坐标
    double projX = x1 + t * (x2 - x1);
    double projY = y1 + t * (y2 - y1);
    // 返回点到投影点的距离
    return sqrt(pow(px - projX, 2) + pow(py - projY, 2));
}
bool Map::isPointBetweenEndpoints(int px, int py, int x1, int y1, int x2, int y2, int extension) {
    int minX = std::min(x1, x2) - extension;
    int maxX = std::max(x1, x2) + extension;
    int minY = std::min(y1, y2) - extension;
    int maxY = std::max(y1, y2) + extension;

    return (px >= minX && px <= maxX && py >= minY && py <= maxY);
}
int Map::findEdgeAt(int x, int y) {
    const int edgeTolerance = 8; // 边的点击容差

    for (int i = 0; i < 72; i++) {
        Edge edge = edges[i];
        // 计算点到线段的距离
        double distance = pointToLineDistance(x, y, edge.x1, edge.y1, edge.x2, edge.y2);
        // 检查是否在线段范围内且距离足够近
        if (distance < edgeTolerance &&
            isPointBetweenEndpoints(x, y, edge.x1, edge.y1, edge.x2, edge.y2, edgeTolerance * 2)) {
            return i;
        }
    }
    return -1; // 未找到
}

void Map::placeRobber(int pieceIndex) {
    // 移除所有地块的强盗
    for (int i = 0; i < 19; i++) {
        pieces[i].hasrobber = false;
    }
    // 在新位置放置强盗
    if (pieceIndex >= 0 && pieceIndex < 19) {
        pieces[pieceIndex].hasrobber = true;
    }
}
bool Map::canBuildSettlement(int vertexIndex, int playerId) {
    if(vertexIndex < 0 || vertexIndex >= 54) {
        return false;
    }
    Vertex& v= vertices[vertexIndex];
    for(int n:v.neighbourVertices) {
        if(vertices[n].buildingType!=0) {
            return false;
        }
    }
    bool hasconnectroads = false;
    for(int e:v.neighbourEdges) {
        if(edges[e].owner==playerId) {
            hasconnectroads = true;
            break;
        }
    }
    if(hasconnectroads) {
        return true;
    }
    return false;
}// 判断是否可以建造村庄

bool Map::canBuildCity(int vertexIndex, int playerId) {
    if(vertexIndex < 0 || vertexIndex >= 54) {
        return false;
    }
    Vertex& v= vertices[vertexIndex];
    if(v.buildingType==1&&v.owner==playerId) {
        return true;
    }
    return false;
}
// 判断是否可以建造城市
bool Map::canBuildRoad(int edgeIndex, int playerId) {
    if(edgeIndex < 0 || edgeIndex >= 72) {
        return false;
    }
    Edge& e= edges[edgeIndex];
    if(e.owner!=-1){
        return false;
    }
    if(findVertexAt(e.x1, e.y1)<0&&findVertexAt(e.x2,e.y2)<0) {
        return false;
    }
    int v1= findVertexAt(e.x1, e.y1);
    int v2= findVertexAt(e.x2, e.y2);
    if(vertices[v1].owner==playerId||vertices[v2].owner==playerId) {
        return true;
    }
    for(int n:vertices[v1].neighbourEdges) {
        if(edges[n].owner==playerId) {
            return true;
        }
    }
    for(int n:vertices[v2].neighbourEdges) {
        if(edges[n].owner==playerId) {
            return true;
        }
    }
    return false;
}// 判断是否可以建造道路

bool Map::buildSettlement(int vertexIndex, int playerId) {
    if(canBuildSettlement(vertexIndex, playerId)) {
        vertices[vertexIndex].buildingType=1;
        vertices[vertexIndex].owner=playerId;
        clearHighlights();
        return true;
    }
    return false;
}   // 建造村庄

bool Map::buildCity(int vertexIndex, int playerId) {
    if(canBuildCity(vertexIndex, playerId)) {
        vertices[vertexIndex].buildingType=2;
        vertices[vertexIndex].owner=playerId;
        clearHighlights();
        return true;
    }
    return false;
}          // 建造城市

bool Map::buildRoad(int edgeIndex, int playerId) {
    if(canBuildRoad(edgeIndex, playerId)) {
        edges[edgeIndex].owner=playerId;
        clearHighlights();
        return true;
    }
    return false;
}         // 建造道路
std::vector<int> Map::getPiecesToVertex(int vertexIndex){
    std::vector<int> neighbourPieces;
    if(vertexIndex < 0||vertexIndex >= 54){
        return neighbourPieces;
    }
    return vertices[vertexIndex].neighbourPieces;
}
std::vector<int> Map::getPiecesToEdge(int edgeIndex){
    std::vector<int> neighbourPieces;
    if(edgeIndex < 0||edgeIndex >= 72){
        return neighbourPieces;
    }
    return edges[edgeIndex].neighbourPieces;
}
std::vector<int> Map::getVerticesToVertex(int vertexIndex){
    std::vector<int> neighbourVertices;
    if(vertexIndex<0||vertexIndex>=54){
        return neighbourVertices;
    }
    return vertices[vertexIndex].neighbourVertices;
}
std::vector<int> Map::getEdgesToVertex(int vertexIndex){
    std::vector<int> neighbourEdges;
    if(vertexIndex<0||vertexIndex>=54){
        return neighbourEdges;
    }
    return vertices[vertexIndex].neighbourEdges;
}
std::vector<int> Map::getBuildableVertices(int playerId) {
    std::vector<int> buildableVertices;
    for (int i = 0; i < 54; i++) {
        if (canBuildSettlement(i, playerId)) {
            buildableVertices.push_back(i);
        }
    }

    return buildableVertices;
}
std::vector<int> Map::getBuildableEdges(int playerId){
    std::vector<int> buildableEdges;
    for (int i = 0; i < 72; i++) {
        if (canBuildRoad(i, playerId)) {
            printf("succesfully build a road\n");
            buildableEdges.push_back(i);
        }
    }
    return buildableEdges;

}
std::vector<int> Map::getEdgesToEdges(int edgeIndex) {
    std::vector<int> neighbourEdges;
    if (edgeIndex < 0 || edgeIndex >= 72) {
        return neighbourEdges;
    }
    // 获取边的两个端点
    int x1 = edges[edgeIndex].x1;
    int x2 = edges[edgeIndex].x2;
    int y1 = edges[edgeIndex].y1;
    int y2 = edges[edgeIndex].y2;
    // 找到对应的顶点索引
    int index1 = findVertexAt(x1, y1);
    int index2 = findVertexAt(x2, y2);
    // 获取两个顶点相邻的边
    std::vector<int> neighbourEdges1 = getEdgesToVertex(index1);
    std::vector<int> neighbourEdges2 = getEdgesToVertex(index2);
    // 合并并去重
    std::vector<int> allEdges;
    allEdges.insert(allEdges.end(), neighbourEdges1.begin(), neighbourEdges1.end());
    allEdges.insert(allEdges.end(), neighbourEdges2.begin(), neighbourEdges2.end());
    // 去重并排除自身
    for (int edge : allEdges) {
        if (edge != edgeIndex &&
            std::find(neighbourEdges.begin(), neighbourEdges.end(), edge) == neighbourEdges.end()) {
            neighbourEdges.push_back(edge);
        }
    }
    return neighbourEdges;
}
void Map::calculateVerticesAndEdges(){
    for(int i=0;i<54;i++){
        vertices[i]=Vertex();
    }
    for(int i=0;i<72;i++){
        edges[i]=Edge();
    }
    int vertexIndex=0,edgeIndex=0;
    //计算所有六边形的边和顶点
    for(int pieceIndex=0;pieceIndex<19;pieceIndex++){
        MapPiece& piece = pieces[pieceIndex];
        int x = piece.x;
        int y = piece.y;
        int size = Size; //六边形外接圆半径
        //计算六边形的六个顶点坐标
     //   printf("%dcentre:%d %d\n",pieceIndex,x,y);
        myPOINT hexVertices[6];
        for(int i=0;i<6;i++){
            double angle = 2*PI/6*i - PI/6;
            hexVertices[i].x = x + size * cos(angle);
            hexVertices[i].y = y + size * sin(angle);
       //    printf("%d:%f %f cosx:%f sinx:%f\n",pieceIndex,hexVertices[i].x,hexVertices[i].y,cos(angle),sin(angle));
        }
      //  printf("sec%d\n",pieceIndex);
        //处理顶点
        for (int i = 0; i < 6; i++) {
            myPOINT& currentVertex = hexVertices[i];
            // 检查是否已存在相同坐标的顶点
            int existingVertexIndex = -1;

            for (int j = 0; j < vertexIndex; j++) {
                if (abs(vertices[j].x - currentVertex.x) < 10 &&
                    abs(vertices[j].y - currentVertex.y) < 10) {
                    existingVertexIndex = j;
                    break;
                }
            }
            if (existingVertexIndex == -1) {
                // 不存在该位置的顶点，创建新顶点
                vertices[vertexIndex].x = currentVertex.x;
                vertices[vertexIndex].y = currentVertex.y;
                vertices[vertexIndex].neighbourPieces.push_back(pieceIndex);
                vertexIndex++;
           //     printf("vertex%d\n",vertexIndex-1);
            } else {
                // 添加到现有顶点
                vertices[existingVertexIndex].neighbourPieces.push_back(pieceIndex);
          //      printf("novertex\n");
            }

        }
        //处理边
        for (int i = 0; i < 6; i++) {
            int nextI = (i + 1) % 6;
            myPOINT& startVertex = hexVertices[i];
            myPOINT& endVertex = hexVertices[nextI];

            // 检查是否已存在相同的边
            int existingEdgeIndex = -1;
            for (int j = 0; j < edgeIndex; j++) {
                if ((abs(edges[j].x1-startVertex.x) <=5 && abs(edges[j].y1 == startVertex.y) <= 5 &&
                     abs(edges[j].x2 - endVertex.x)<=5 && abs(edges[j].y2 - endVertex.y)<=5) ||
                        (abs(edges[j].x2-startVertex.x) <=5 && abs(edges[j].y2 == startVertex.y) <= 5 &&
                         abs(edges[j].x1 - endVertex.x)<=5 && abs(edges[j].y1 - endVertex.y)<=5)) {
                    existingEdgeIndex = j;
                    break;
                }
            }

            if (existingEdgeIndex == -1) {
                // 该位置不存在已有边，创建新边
                edges[edgeIndex].x1 = startVertex.x;
                edges[edgeIndex].y1 = startVertex.y;
                edges[edgeIndex].x2 = endVertex.x;
                edges[edgeIndex].y2 = endVertex.y;
                edges[edgeIndex].neighbourPieces.push_back(pieceIndex);
                edgeIndex++;
            } else {
                // 添加到现有边
                edges[existingEdgeIndex].neighbourPieces.push_back(pieceIndex);
            }
           // printf("edge:%d (%d,%d),(%d,%d)\n",edgeIndex,edges[edgeIndex-1].x1,edges[edgeIndex-1].y1,edges[edgeIndex-1].x2,edges[edgeIndex-1].y2);
        }
    }
}
void Map::establishVertexNeighbours(){
    for (int i = 0; i < 72; i++) {
        Edge& edge = edges[i];
        // 找到边的两个端点顶点
        int vertex1 = findVertexAt(edge.x1, edge.y1);
        int vertex2 = findVertexAt(edge.x2, edge.y2);
        if (vertex1 != -1 && vertex2 != -1) {
            // 添加相互的相邻关系
            if (std::find(vertices[vertex1].neighbourVertices.begin(),
                          vertices[vertex1].neighbourVertices.end(), vertex2) == vertices[vertex1].neighbourVertices.end()) {
                vertices[vertex1].neighbourVertices.push_back(vertex2);
            }

            if (std::find(vertices[vertex2].neighbourVertices.begin(),
                          vertices[vertex2].neighbourVertices.end(), vertex1) == vertices[vertex2].neighbourVertices.end()) {
                vertices[vertex2].neighbourVertices.push_back(vertex1);
            }
        }
    }
}
void Map::establishEdgeVertexRelations(){
    for (int i = 0; i < 72; i++) {
        Edge& edge = edges[i];
        int vertex1 = findVertexAt(edge.x1, edge.y1);
        int vertex2 = findVertexAt(edge.x2, edge.y2);
        if (vertex1 != -1) {
            if (std::find(vertices[vertex1].neighbourEdges.begin(),
                          vertices[vertex1].neighbourEdges.end(), i) == vertices[vertex1].neighbourEdges.end()) {
                vertices[vertex1].neighbourEdges.push_back(i);
            }
        }

        if (vertex2 != -1) {
            if (std::find(vertices[vertex2].neighbourEdges.begin(),
                          vertices[vertex2].neighbourEdges.end(), i) == vertices[vertex2].neighbourEdges.end()) {
                vertices[vertex2].neighbourEdges.push_back(i);
            }
        }
    }
}
std::vector<std::vector<std::pair<ResourceType, int>>> Map::distributeResources(int diceResult) {
    std::vector<std::vector<std::pair<ResourceType, int>>> playerResources(4);

    // 如果骰子是7，触发强盗，不分配资源,返回空
    if (diceResult == 7) {
        return playerResources;
    }
    for (int vertexIndex = 0; vertexIndex < 54; vertexIndex++) {
        Vertex& vertex = vertices[vertexIndex];
        // 如果顶点有建筑
        if (vertex.buildingType > 0 && vertex.owner != -1) {
            int playerId = vertex.owner;
            // 获取顶点相邻的地块
            std::vector<int> neighbourPieces = getPiecesToVertex(vertexIndex);
            // 检查每个相邻地块
            for (int pieceIndex : neighbourPieces) {
                MapPiece& piece = pieces[pieceIndex];
                // 如果地块数字匹配骰子结果且没有强盗
                if (piece.num == diceResult && !piece.hasrobber && piece.r != r6) {
                    // 根据建筑类型决定资源数量,如果被强盗占领，资源不增加
                    int resourceCount = (vertex.buildingType == 1) ? 1 : 2; // 村庄1个，城市2个
                    playerResources[playerId].push_back({piece.r, resourceCount});

                }
            }
        }
    }

    return playerResources;
}
void Map::set_button() {
    int startX = 50;           // 起始X坐标
    int startY = 500;          // 起始Y坐标
    int buttonWidth = 100;     // 按钮宽度
    int buttonHeight = 30;     // 按钮高度
    int buttonSpacing = 10;    // 按钮间距
    // 设置取消建造模式按钮
    buildButtons[0].x = startX;
    buildButtons[0].y = startY;
    buildButtons[0].width = buttonWidth;
    buildButtons[0].height = buttonHeight;
    buildButtons[0].text = "取消建造模式";
    buildButtons[0].isActive = false;
    buildButtons[0].mode = MODE_NONE;
    // 设置建筑按钮（村庄）
    buildButtons[1].x = startX + buttonWidth + buttonSpacing;
    buildButtons[1].y = startY;
    buildButtons[1].width = buttonWidth;
    buildButtons[1].height = buttonHeight;
    buildButtons[1].text = "建造村庄";
    buildButtons[1].isActive = false;
    buildButtons[1].mode = MODE_SETTLEMENT;
    // 设置道路按钮
    buildButtons[2].x = startX + (buttonWidth + buttonSpacing) * 2;
    buildButtons[2].y = startY;
    buildButtons[2].width = buttonWidth;
    buildButtons[2].height = buttonHeight;
    buildButtons[2].text = "建造道路";
    buildButtons[2].isActive = false;
    buildButtons[2].mode = MODE_ROAD;
    // 设置城市按钮
    buildButtons[3].x = startX + (buttonWidth + buttonSpacing) * 3;
    buildButtons[3].y = startY;
    buildButtons[3].width = buttonWidth;
    buildButtons[3].height = buttonHeight;
    buildButtons[3].text = "升级城市";
    buildButtons[3].isActive = false;
    buildButtons[3].mode = MODE_CITY;

}
void Map::initMap(IMAGE A) {
    set_bg(A);
    rng.seed(randomSeed);
    // 建立19个板块
    set_map();
    // 随机分布资源
    fromPiece = 0;
    shuffleResources();
    // 设置数字编号
    setNumbers();
    // 计算顶点和边
    calculateVerticesAndEdges();
    init_player();
    set_harbour();
    //建立关系
    establishVertexNeighbours();
    establishEdgeVertexRelations();
    // 设置按钮
 //   set_button();
    // 初始化玩家
    loadImage();   //预加载所有图片
    currentPlayer = 4;
    currentMode = MODE_NONE;
    isInitialized = true;
}
void Map::init_player() {
    for(int i=0;i<72;i++){
        edges[i].owner = -1;
    }
    for(int i=0;i<54;i++){
        vertices[i].owner = -1;
    }
}
void Map::set_map() {
    int centerX = screenWidth/2;
    int centerY = screenHeight/2;
    double hexSize = Size*cos(PI/6);
    // 第1层（中心）：1个
    pieces[0] = {centerX, centerY, r1, Size, 0, true,false};
    // 第2层（中间环）：6个
    int layer2Radius = hexSize * 2;
    int layer2OffsetY = static_cast<int>(hexSize * sqrt(3));
    pieces[1] = {centerX + layer2Radius, centerY, r1, Size, 0, false,false};
    pieces[2] = {centerX + layer2Radius/2, centerY + layer2OffsetY, r1, Size, 0, false,false};
    pieces[3] = {centerX - layer2Radius/2, centerY + layer2OffsetY, r1, Size, 0, false,false};
    pieces[4] = {centerX - layer2Radius, centerY, r1, Size, 0, false,false};
    pieces[5] = {centerX - layer2Radius/2, centerY - layer2OffsetY, r1, Size, 0, false,false};
    pieces[6] = {centerX + layer2Radius/2, centerY - layer2OffsetY, r1, Size, 0, false,false};
    // 第3层（外环）：12个
    double layer3Radius = hexSize * 4;
    pieces[7]  = {centerX + static_cast<int>(layer3Radius), centerY, r1, Size, 0, false,false};
    pieces[8]  = {centerX + static_cast<int>(3*hexSize), centerY + layer2OffsetY, r1, Size, 0, false,false};
    pieces[9]  = {centerX + static_cast<int>(hexSize*2), centerY + 3*Size, r1, Size, 0, false,false};
    pieces[10] = {centerX , centerY + 3*Size, r1, Size, 0, false,false};
    pieces[11] = {centerX - layer2Radius, centerY + 3*Size, r1, Size, 0, false,false};
    pieces[12] = {centerX - static_cast<int>(3*hexSize), centerY + layer2OffsetY, r1, Size, 0, false,false};
    pieces[13] = {centerX - static_cast<int>(layer3Radius), centerY , r1, Size, 0, false,false};
    pieces[14] = {centerX - static_cast<int>(3*hexSize), centerY - layer2OffsetY, r1, Size, 0, false,false};
    pieces[15] = {centerX - layer2Radius, centerY - 3*Size, r1, Size, 0, false,false};
    pieces[16] = {centerX, centerY - 3*Size, r1, Size, 0, false,false};
    pieces[17] = {centerX + layer2Radius, centerY - 3*Size, r1, Size, 0, false,false};
    pieces[18] = {centerX + static_cast<int>(3*hexSize), centerY - layer2OffsetY, r1, Size, 0, false,false};

}
void Map::setNumbers() {
    std::vector<int> numbers = {6, 3, 11, 9, 4, 5, 10, 9, 12, 11, 4, 8, 10, 5, 2, 6, 3, 8};
    int numberIndex = 0;
    pieces[0].num = -1 ;
    // 为每个非沙漠地块分配数字
    for (int i = 1; i < 19; i++) {
        if (pieces[i].r != r6) { // 不是沙漠
            pieces[i].num = numbers[numberIndex++];
        } else {
            pieces[i].num = 0;
            pieces[i].hasrobber = true; // 沙漠初始有强盗
        }
      //  printf("vertex%d:%d\n",i,pieces[i].num);
    }


}

void Map::shuffleResources() {
    std::shuffle(std::begin(resourcetype), std::end(resourcetype), rng);
    pieces[0].r=r6;
    pieces[0].hasrobber = true;
    pieces[0].num=0;
    int t=1;
    for (int i=0;i<19;i++ ) {
        if (resourcetype[i] == r6) {
            continue;
        }
        pieces[t].r=resourcetype[i];
        pieces[t].hasrobber = false;
        pieces[t].num = -1;
        t++;
      //  printf("%d:%d\n",i,pieces[i].r);

    }
}
Map::Map(unsigned int seed) : randomSeed(seed), isInitialized(false), currentMode(MODE_NONE), currentPlayer(0) {
    // 初始化随机数生成器
    rng.seed(randomSeed);
    // 初始化地块
    for (int i = 0; i < 19; i++) {
        pieces[i] = {0, 0, r1, 0, 0, false};
    }
    // 初始化顶点
    for (int i = 0; i < 54; i++) {
        vertices[i] = {0, 0, {}, {}, {}, 0, -1, false};
    }
    // 初始化边
    for (int i = 0; i < 72; i++) {
        edges[i] = {0, 0, 0, 0, {}, -1, false};
    }
    // 初始化按钮
    for (int i = 0; i < 4; i++) {
        buildButtons[i] = {0, 0, 0, 0, "", false, MODE_NONE};
    }
}
void Map::drawHexagonBorder(int x, int y, int size) {
    if (size <= 0) return;
    POINT vertices[6];
    double radianStep = 2 * PI / 6;
    double startRadian = PI / 6;
    for (int i = 0; i < 6; i++) {
        double radian = startRadian + i * radianStep;
        vertices[i].x = static_cast<LONG>(x + size * cos(radian));
        vertices[i].y = static_cast<LONG>(y + size * sin(radian));
    }
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, Size);
 //   polygon(vertices, 6);
}

// 获取资源对应的颜色（适配 MinGW + EasyX，避免未定义颜色常量）
COLORREF Map::getResourceColor(ResourceType type) {
    switch (type) {
        case r1:  // 森林（r1 = 森林）
            return RGB(0, 100, 0);    // 深绿色（替代 DARKGREEN，MinGW 兼容）
        case r2:  // 丘陵（r2 = 丘陵）
            return RGB(165, 42, 42);  // 棕色（替代 BROWN，精准匹配卡坦岛风格）
        case r3:  // 山脉（r3 = 山脉）
            return RGB(128, 128, 128); // 中灰色（替代 GRAY，MinGW 兼容）
        case r4:  // 草地（r4 = 草地）
            return RGB(34, 139, 34);   // 翠绿色（比默认 GREEN 更贴近草地）
        case r5:  // 稻田（r5 = 稻田）
            return RGB(255, 215, 0);   // 金色（比默认 YELLOW 更贴近小麦）
        case r6:  // 沙漠（r6 = 沙漠）
            return RGB(245, 245, 220); // 米白色（比 LIGHTGRAY 更贴近沙漠）
        default:  // 默认颜色（防止枚举值异常）
            return WHITE;
    }
}
// 获取资源名称
const char* Map::getResourceName(ResourceType type) {
    switch (type) {
        case r1:  // 森林（r1 = 森林）
            return "forest" ;
        case r2:  // 丘陵（r2 = 丘陵）
            return "hill";
        case r3:  // 山脉（r3 = 山脉）
            return "mountain";
        case r4:  // 草地（r4 = 草地）
            return "pasture";
        case r5:  // 稻田（r5 = 稻田）
            return "field";
        case r6:  // 沙漠（r6 = 沙漠）
            return "desert";
        default:  // 防止异常
            return "Wrong resource";
    }
}
//获取数字颜色————概率越高颜色越醒目
COLORREF Map::getNumberColor(int number) {
    switch (number) {
        case 2:  // 概率最低（1种组合）
            return RGB(255, 200, 200); // 浅粉色
        case 3:  // 概率低（2种组合）
            return RGB(255, 160, 160); // 粉色
        case 4:  // 概率中低（3种组合）
            return RGB(255, 120, 120); // 浅红色
        case 5:  // 概率中（4种组合）
            return RGB(255, 80, 80);   // 红色
        case 6:  // 概率最高（5种组合）
            return RGB(220, 20, 20);   // 深红色（最醒目）
        case 8:  // 概率最高（5种组合）
            return RGB(220, 20, 20);   // 深红色（和6保持一致）
        case 9:  // 概率中（4种组合）
            return RGB(255, 80, 80);   // 红色
        case 10: // 概率中低（3种组合）
            return RGB(255, 120, 120); // 浅红色
        case 11: // 概率低（2种组合）
            return RGB(255, 160, 160); // 粉色
        case 12: // 概率最低（1种组合）
            return RGB(255, 200, 200); // 浅粉色
        default: // 异常数字（如沙漠板块的0）
            return WHITE;
    }
}
// 检查数字放置是否有效（利用工具函数优化逻辑）
bool Map::isValidNumberPlacement(int pieceIndex, int number) {
    if (pieceIndex < 0 || pieceIndex >= 19) {
        return false;
    }
    if (number < 2 || number > 12 || number == 7) {
        return false;
    }
    if (pieces[pieceIndex].r == r6) {
        return false;
    }
    std::vector<int> currentPieceVertices;
    for (int v = 0; v < 54; v++) { // 遍历所有 54 个顶点
        std::vector<int> piecesOfVertex = getPiecesToVertex(v);
        if (std::find(piecesOfVertex.begin(), piecesOfVertex.end(), pieceIndex) != piecesOfVertex.end()) {
            currentPieceVertices.push_back(v);
        }
    }
    std::vector<int> adjacentPieces;
    for (int v : currentPieceVertices) {
        std::vector<int> piecesOfVertex = getPiecesToVertex(v);
        for (int p : piecesOfVertex) {
            if (p != pieceIndex && std::find(adjacentPieces.begin(), adjacentPieces.end(), p) == adjacentPieces.end()) {
                adjacentPieces.push_back(p);
            }
        }
    }
    for (int adjPiece : adjacentPieces) {
        if (pieces[adjPiece].num == number) {
            return false;
        }
    }
    return true;
}

// 找到沙漠地块
int Map::findDesertPiece() {
    for (int i = 0; i < 19; i++) {
        if(pieces[i].r==r6) {
            return i;
        }
    }
    return -1;
}

// 绘制顶点
void Map::drawVertex(int vertexIndex) {
    if (vertexIndex < 1 || vertexIndex >= 54) return;
    Vertex v= vertices[vertexIndex];
    int x=v.x,y=v.y;
    // 2. 绘制顶点基础样式（根据状态区分）
    const int vertexRadius = 5;  // 顶点基础半径（像素）
    if (v.isHighlighted) {
        // 可建造状态：高亮金色圆圈（提示玩家可建村庄）
        setfillcolor(RGB(255, 215, 0)); // 金色填充
        setlinecolor(RGB(255, 180, 0)); // 深金色边框
        fillcircle(x, y, 7*vertexRadius); // 放大1号，更醒目
    } else if (v.buildingType == 0 && !v.isHarbour) {
        // 无建筑且非港口：灰色小圆圈（默认状态）
        setfillcolor(LIGHTGRAY);
        setlinecolor(BROWN);
        fillcircle(x, y, vertexRadius);
    }
    // 3. 绘制港口标记（若该顶点是港口，isNeighbour 标记为 true）
    if (v.isHarbour) {
        setfillcolor(RGB(135, 206, 235)); // 天蓝色填充（港口颜色）
        setlinecolor(BLUE);               // 蓝色边框
        // 港口绘制为小正方形（与普通顶点区分）
        int portSize = 8;
        fillrectangle(
                x - portSize/2, y - portSize/2,
                x + portSize/2, y + portSize/2
        );
        rectangle(
                x - portSize/2, y - portSize/2,
                x + portSize/2, y + portSize/2
        );
    }
    // 4. 绘制建筑（村庄/城市）
    const int settlementSize = 10;  // 村庄大小
    const int citySize = 14;        // 城市大小（比村庄大）
    switch (v.buildingType) {
        case 1: {// 村庄（小圆圈，玩家颜色）
            // 根据玩家ID设置颜色（0=红色，1=蓝色，2=黄色，3=绿色，可扩展）
            COLORREF settlementColor = WHITE;
            setfillcolor(settlementColor);
            setlinecolor(BLACK); // 黑色边框，更清晰
            fillcircle(x, y, settlementSize);
            circle(x, y, settlementSize);
            break;}
        case 2: // 城市（大正方形，玩家颜色加深）
            {COLORREF cityColor = LIGHTCYAN;
            setfillcolor(cityColor);
            setlinecolor(BLACK);
            fillrectangle(
                    x - citySize/2, y - citySize/2,
                    x + citySize/2, y + citySize/2
            );
            rectangle(
                    x - citySize/2, y - citySize/2,
                    x + citySize/2, y + citySize/2
            );
            break;}
        default: //do nothing
            break;
    }
}
// 绘制边
void Map::drawEdge(int edgeIndex) {
    if (edgeIndex < 0 || edgeIndex >= 72) return;
    Edge edge = edges[edgeIndex];
    int x1 = edge.x1, y1 = edge.y1;
    int x2 = edge.x2, y2 = edge.y2;
    const int defaultWidth = 6;    // 未占用道路宽度（细灰边）
    const int builtWidth = 10;      // 已建造道路宽度（加粗，玩家颜色）
    const int highlightWidth = 20;  // 可建造道路宽度（金色，醒目）
    if(edge.isHighlighted) {
        printf("edge[%d]:ishighlighted\n",edgeIndex);
        setlinecolor(RGB(255, 215, 0)); // gold
        setlinestyle(PS_SOLID, highlightWidth);
    }else if (edge.owner!= -1) {
        COLORREF playe_of_Color = RED;
        setlinecolor(playe_of_Color); // 复用玩家颜色函数
        setlinestyle(PS_SOLID, builtWidth);
       // printf("linecolor:red\n");
    }else { setlinecolor(RGB(120,67,21));
        setlinestyle(PS_SOLID, defaultWidth);
      //  printf("linecolor:black\n");
    }
    line(x1, y1, x2, y2);
   // printf("drawedge:%d\n",edgeIndex);
}
void Map::clearHighlights() {
    for (int e = 0; e < 72; e++) { // 卡坦岛共 72 条边
        if (edges[e].isHighlighted) {
            edges[e].isHighlighted = false;
            drawEdge(e);
        }
    }
    // 2. 清除所有顶点（Vertex）的高亮
    for (int v = 0; v < 54; v++) {
        if (vertices[v].isHighlighted) {
            vertices[v].isHighlighted = false;
            drawVertex(v);
        }
    }
}// 清除所有高亮

// 随机数工具函数
// 生成区间随机整数
int Map::randomInt(int min, int max) {
    if (min > max) {
        std::swap(min, max);
    }
    // 使用类内的 mt19937 随机数生成器（已在构造函数初始化）
    // uniform_int_distribution 生成 [min, max] 区间的均匀分布整数
    std::uniform_int_distribution<> dist(min, max);
    return dist(rng);
}
// 生成区间随机浮点数
double Map::randomDouble(double min, double max) {
    if (min > max) {
        std::swap(min, max);
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}
void Map::drawAllRoads() {
    for (int i = 0; i < 72; i++) {
        Edge& edge = edges[i];
        if (edge.owner != -1) { // 有道路
            drawEdge(i);
        }
    }
}
void Map::drawAllBuildings() {
    for (int i = 0; i < 54; i++) {
        Vertex& vertex = vertices[i];
        if (vertex.buildingType != 0) { // 有建筑
            if(vertex.buildingType==1){
                drawSettlement(vertex.x,vertex.y,vertex.owner);
            }else{
                drawSettlement(vertex.x,vertex.y,vertex.owner);
            }
        }
    }
}
void Map::drawAllRobbers() {
    for (int i = 0; i < 19; i++) {
        MapPiece& piece = pieces[i];
        if (piece.hasrobber) {
            drawRobber(piece.x, piece.y);
            printf("draw robbers");
        }
    }
}
void Map::drawHighlights() {
    setlinecolor(RGB(255,215,0));
   for(int i=0;i<72;i++){
       drawEdge(i);
   }
}
void Map::set_bg(IMAGE A){
    bg = A;
}
void Map::drawAll() {
    putimage(0,0,&bg);
    drawMap();
    // 绘制所有道路(已经包含高亮状态)
    drawAllRoads();
    // 绘制所有建筑（村庄和城市）
    drawAllBuildings();
    // 绘制强盗
    drawAllRobbers();
}

// 返回指定玩家当前拥有的资源数量和种类
std::vector<std::pair<ResourceType, int>> Map::getPlayerResources(int playerId) {
    std::vector<std::pair<ResourceType, int>> resources;
    // 初始化资源计数器（6种资源）
    int resourceCounts[6] = {0}; // 对应 r1-r6
    calculateResourcesFromBuildings(playerId, resourceCounts);

    for (int i = 0; i < 6; i++) {
        if (resourceCounts[i] > 0) {
            resources.push_back({static_cast<ResourceType>(i), resourceCounts[i]});
        }
    }

    return resources;
}
void Map::calculateResourcesFromBuildings(int playerId, int resourceCounts[6]) {
    // 遍历所有顶点，找到该玩家的建筑
    for (int vertexIndex = 0; vertexIndex < 54; vertexIndex++) {
        Vertex& vertex = vertices[vertexIndex];
        if (vertex.buildingType > 0 && vertex.owner == playerId) {
            // 获取建筑相邻的地块
            std::vector<int> neighbourPieces = getPiecesToVertex(vertexIndex);
            // 为每个相邻地块添加资源
            for (int pieceIndex : neighbourPieces) {
                MapPiece& piece = pieces[pieceIndex];
                // 排除沙漠和有强盗的地块
                if (piece.r != r6 && !piece.hasrobber) {
                    // 根据建筑类型决定资源数量
                    int resourceAmount = (vertex.buildingType == 1) ? 1 : 2; // 村庄1个，城市2个
                    resourceCounts[piece.r] += resourceAmount;
                }
            }
        }
    }
}
std::vector<bool> Map::GetTradeOption(int playerId) {
    // 7位交易选项: [4:1, 3:1, 木材2:1, 砖块2:1, 矿石2:1, 羊毛2:1, 粮食2:1]
    std::vector<bool> tradeOptions(7, false);
    if (playerId < 1 || playerId > 4) {
        return tradeOptions;
    }
    tradeOptions[0] = true;
    // 3:1交易 需要通用港口
    tradeOptions[1] = hasGeneralPort(playerId);
    // 检查各种资源的2:1交易（需要专用港口）
    for (int resourceType = 0; resourceType < 5; resourceType++) {
        ResourceType type = static_cast<ResourceType>(resourceType);
        tradeOptions[2 + resourceType] = hasSpecialPortForResource(playerId, type);
    }

    return tradeOptions;
}
// 找出最外层的边
std::vector<int> Map::getOuterEdges() {
    std::vector<int> outerEdges;

    // 通过相邻地块数量判断
    for (int i = 0; i < 72; i++) {
        if (edges[i].neighbourPieces.size() == 1) {
            // 只相邻1个地块的边就是最外层边
            outerEdges.push_back(i);
        }
    }


    return outerEdges;
}
void Map::set_harbour() {
    // 初始化所有边的港口状态
    for (int i = 0; i < 72; i++) {
        edges[i].isHarbour = -1; // 默认无港口
    }
    // 直接指定港口位置（确保在最外层且间隔足够）
    std::vector<std::pair<int, int>> harbourAssignments = {
            {34,   0},   //  边 ：通用港口
            {35,  1},   // 边14: 通用港口
            {39,  0},   // 边38: 通用港口
            {43,  3},   // 边56: 通用港口
            {47,   0},   // 边8: 木材专用港口
            {53,  2},   // 边26: 砖块专用港口
            {57,  0},   // 边44: 矿石专用港口
            {64,  4},   // 边62: 羊毛专用港口
            {69,  5}    // 边20: 粮食专用港口
    };
    for (const auto& assignment : harbourAssignments) {
        int edgeIndex = assignment.first;
        int portType = assignment.second;

        if (edgeIndex >= 0 && edgeIndex < 72) {
            edges[edgeIndex].isHarbour = portType;
        }
    }
}
bool Map::hasSpecialPortForResource(int playerId, ResourceType resourceType) {
    // 玩家ID从1到4
    if (playerId < 1 || playerId > 4) {
        return false;
    }
    int playerIndex = playerId - 1;
    // 将资源类型转换为portType（r1->1, r2->2, ..., r5->5）
    int targetPortType = static_cast<int>(resourceType) + 1;
    // 检查所有边
    for (int edgeIndex = 0; edgeIndex < 72; edgeIndex++) {
        Edge& edge = edges[edgeIndex];
        if (edge.isHarbour == targetPortType) {
            // 获取港口边的两个端点
            int vertex1 = findVertexAt(edge.x1, edge.y1);
            int vertex2 = findVertexAt(edge.x2, edge.y2);
            // 检查玩家是否在任意一个端点有建筑
            if ((vertex1 != -1 && vertices[vertex1].buildingType > 0 && vertices[vertex1].owner == playerIndex) ||
                (vertex2 != -1 && vertices[vertex2].buildingType > 0 && vertices[vertex2].owner == playerIndex)) {
                return true;
            }
        }
    }

    return false;
}

bool Map::hasGeneralPort(int playerId) {
    // 玩家ID从1到4
    if (playerId < 1 || playerId > 4) {
        return false;
    }
    int playerIndex = playerId - 1;
    for (int edgeIndex = 0; edgeIndex < 72; edgeIndex++) {
        Edge& edge = edges[edgeIndex];
        if (edge.isHarbour == 0) { // 通用港口
            // 获取港口边的两个端点
            int vertex1 = findVertexAt(edge.x1, edge.y1);
            int vertex2 = findVertexAt(edge.x2, edge.y2);
            // 检查玩家是否在任意一个端点有建筑
            if ((vertex1 != -1 && vertices[vertex1].buildingType > 0 && vertices[vertex1].owner == playerIndex) ||
                (vertex2 != -1 && vertices[vertex2].buildingType > 0 && vertices[vertex2].owner == playerIndex)) {
                return true;
            }
        }
    }

    return false;
}
void Map::drawHarbour(){
    for(int i=0;i<72;i++){
        Edge edge = edges[i];
        if(edge.isHarbour>=0){
            int x1 = edge.x1;
            int x2 = edge.x2;
            int y1 = edge.y1;
            int y2 = edge.y2;
            int x3 = (x1+x2)/2;
            int y3 = (y1+y2)/2;
            int centrex = pieces[0].x;
            int centrey = pieces[0].y;
            double lamda = 0.05;
            double imageX = x3 - HARBOURSIZE/2;
            double imageY = y3 - HARBOURSIZE/2;
            double imagex = imageX + lamda * (imageX - centrex);
            double imagey = imageY + lamda * (imageY - centrey);
            putimage(imagex,imagey,&harbourImage[edge.isHarbour]);
            printf("%d\n",i);
            }
        }
}
void Map::drawRoad(int x,int y,int PlayerID) {
    for(int i=0;i<72;i++) {
        int x1=edges[i].x1,y1=edges[i].y1,x2=edges[i].x2,y2=edges[i].y2;
        if(isPointBetweenEndpoints(x,y,x1,y1,x2,y2,10)) {
            if(edges[i].owner !=-1) {
                return;
            }
            edges[i].owner =PlayerID;
            COLORREF playe_of_Color = RED;
            setlinecolor(playe_of_Color); // 复用玩家颜色函数
            setlinestyle(PS_SOLID, 3);
            return;
        }
    }
}
int Map::longRoadOwner() {
    std::vector<int> playerRoadlengths(4,0);
    std::vector<bool> visited(54,false);
    for(int player=0;player<4;player++) {
        int maxlength=0;
        for(int start=0;start<54;start++) {
            if(edges[start].owner==player&&!visited[start]) {
                int length=dfs(player,start,visited);
                maxlength=std::max(maxlength,length);
            }
        }
        playerRoadlengths[player]=maxlength;
    }
    int maxRoadOwner=-1;
    int maxRoadLength=0;
    for(int player=0;player<4;player++) {
        if(playerRoadlengths[player]>maxRoadLength) {
            maxRoadLength=playerRoadlengths[player];
            maxRoadOwner=player;
        }
    }
    if(maxRoadLength>=5) {
        return maxRoadOwner;
    }
    return -1;
}

int Map::dfs(int play,int start,std::vector<bool>& visited) {
    if(play!=edges[start].owner||visited[start]) {
        return 0;
    }
    visited[start]=true;
    int maxlength=0;
    std::vector<int> neighbourEdges=getEdgesToEdges(start);
    for(int n:neighbourEdges) {
        if(edges[n].owner==play&&!visited[n]) {
            int length=dfs(play,n,visited);
            maxlength=std::max(maxlength,length);
        }
    }
    visited[start]=false;
    return maxlength;
}

bool Map::checkButtonClick(int x, int y) {
    for(int i=0;i<4;i++) {
        Button btn=buildButtons[i];
        if(x>=btn.x-btn.width/2&&x<=btn.x+btn.width/2&&y>=btn.y-btn.height/2&&y<=btn.y+btn.height/2) {
            return true;
        }
    }
    return false;
}   // 检查按钮点击

void Map::setBuildMode(BuildMode mode) {
    currentMode=mode;
    updateHighlights();
}      // 设置建造模式

void Map::updateHighlights() {
    clearHighlights();
    switch(currentMode) {
        case MODE_ROAD:{
            std::vector<int> buildable_edges=getBuildableEdges(currentPlayer);
            for(int n:buildable_edges) {
                edges[n].isHighlighted=true;
                printf("edge[%d] is highlighted\n",n);
            }
            break;
        }
        case MODE_SETTLEMENT: {
            std::vector<int> buildable_vertices=getBuildableVertices(currentPlayer);
            for(int n:buildable_vertices) {
                vertices[n].isHighlighted=true;
            }
            break;
        }
        case MODE_CITY: {
            for(int i=0;i<54;i++) {
                if(vertices[i].owner==currentPlayer&&vertices[i].buildingType==1) {
                    vertices[i].isHighlighted=true;
                }
            }
            break;
        }
        default:
            break;
    }
}                // 更新高亮显示

void Map::drawMap() {
    for(int i=0;i<19;i++) {
        drawPieces(pieces[i].x,pieces[i].y,Size,pieces[i].r,pieces[i].num,pieces[i].hasrobber);
    }
    for(int i=0;i<72;i++) {
        drawEdge(i);}
    for(int i=0;i<54;i++) {
    drawVertex(i);
  //    printf("drawvertex:%d\n",i);
   }
    drawHarbour();

}//绘制地图

void Map::drawPieces(int x, int y, int size, ResourceType type, int number , bool hasRobber ) {

  //  printf("suc:%s:\n",imagePaths[type]);
     int width = PICTUREWIDTH;
     int height = PICTUREHEIGHT;
    putimage(x-width/2,y-height/2,&pieceImage[type]);
    // 绘制六边形边框
    drawHexagonBorder(x, y, size);
    drawNumberCircle(x,y,number);
    // 绘制强盗（如果有）
}
bool Map::handleBuildRequest(BuildingType type,int playerId,int mousex,int mousey,bool isBeginning){
    if(type == road){
        int edgeindex = findEdgeAt(mousex,mousey);
        return buildRoad(edgeindex,playerId);
    }
    int vertexindex = findVertexAt(mousex,mousey);
    if(type == village){
        if(isBeginning){
            return buildVillage(vertexindex,playerId);
        }
         return buildSettlement(vertexindex,playerId);
    }
    if(type == city){
        return buildCity(vertexindex,playerId);
    }
    return false;
}
void Map::handleBuildRequest(BuildingType type,int playerId,bool isBeginning){
    switch(type){
        case road:currentMode = MODE_ROAD;printf("setROAD_MODE\n");break;
        case village:currentMode = MODE_SETTLEMENT;break;
        case city:currentMode = MODE_CITY;break;
        default:break;
    }
    updateHighlights();
}
bool Map::moveRobber() {
    pieces[fromPiece].hasrobber = false;

    int x1=pieces[fromPiece].x;
    int y1=pieces[fromPiece].y;
    int x2=pieces[toPiece].x;
    int y2=pieces[toPiece].y;
    static int innu=0;
    innu++;
    int index = innu/4;

    static float progress=0.0f;
    float step=1.0/30;
    while(progress<1.0f) {
        if (innu % 4 == 0) {
            progress += step;
        }
        int currentX = x1 + (int) ((x2 - x1) * progress);
        int currentY = y1 + (int) ((y2 - y1) * progress);
        int t = index % 2;
        // 绘制图片，使用浮点数计算位置
        if (x1 > x2) {
            putimage(currentX, currentY, &dragonImage[t]);
            return false;
        } else {
            putimage(currentX, currentY, &dragonImage[t + 2]);
            return false;
        }
    }

    pieces[toPiece].hasrobber = true;
    fromPiece = toPiece;
    return true;
}
bool Map::buildVillage(int vertexIndex,int playerId){
    if(vertexIndex>=0&&vertexIndex<54){
    vertices[vertexIndex].buildingType=1;
    vertices[vertexIndex].owner=playerId;
        clearHighlights();
    return true;
    }else{
        clearHighlights();
        return false;
    }
}

void Map::handleRobberMove(){
    for(int i=0;i<19;i++){
        if(!pieces[i].hasrobber) {
            pieces[i].isHighlighted = true;
        }
    }
}
std::pair<bool,std::vector<int>> Map::handleRobberMove(int mousex,int mousey){
    std::vector<int> players;
    int destination = findPieceAt(mousex,mousey);
    toPiece = destination;
    if(destination == -1 || pieces[destination].hasrobber) return make_pair(false,players);
    int start;
    for(int i = 0;i<19;i++){
        if(pieces[i].hasrobber){
            start = i;
            break;
        }
    }

    for(int vertexIndex = 0;vertexIndex < 54;vertexIndex++){
        int player = vertices[vertexIndex].owner;
        if(player==-1){
            continue;
        }
        for(int i: getPiecesToVertex(vertexIndex)){
            if(i==destination&&std::find(players.begin(),players.end(),player)==players.end()){
                players.push_back(player);
            }
        }
    }
   return make_pair(true,players);
}
void Map::loadImage() {
    loadimage(&bg,IMAGE_PATH"background.jpg",screenWidth,screenHeight);
    const char* imagePaths[6] = {
            IMAGE_PATH"forest.png",    // r1 森林
            IMAGE_PATH"hill.png",      // r2 丘陵
            IMAGE_PATH"mountain.png",  // r3 山脉
            IMAGE_PATH"pasture.png",   // r4 草地
            IMAGE_PATH"field.png",     // r5 稻田
            IMAGE_PATH"desert1.png"     // r6 沙漠
    };

    for(int type = 0;type< 6;type++){
    loadimage(&pieceImage[type],imagePaths[type],PICTUREWIDTH,PICTUREHEIGHT);
    }
    for(int i=1;i<=4;i++){
        char s[50]="";
    sprintf_s(s,"%sdragon%d.png",IMAGE_PATH,i);
    loadimage(&dragonImage[i-1],s,ROBBERWIDTH,ROBBERHEIGHT);
    }
    for(int i=0;i<6;i++){
        char s[50]="";
        sprintf_s(s,"%sharbour%d.png",IMAGE_PATH,i);
        loadimage(&harbourImage[i],s,HARBOURSIZE,HARBOURSIZE);
    }
    for(int i=1;i<=4;i++){
        char s[50]="";
        sprintf_s(s,"%svillage%d.png",IMAGE_PATH,i);
        loadimage(&villageImage[i-1],s,VILLAGEWIDTH,VILLAGEHEIGHT);
    }
    for(int i=1;i<=4;i++){
        char s[50]="";
        sprintf_s(s,"%scity%d.png",IMAGE_PATH,i);
        loadimage(&cityImage[i-1],s,CITYWIDTH,CITYHEIGHT);
    }

}