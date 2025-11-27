#pragma once

/* -----------------------
    基本类型与枚举
    ----------------------- */
//enum class ResourceType { Wood, Brick, Ore, Sheep, Wheat, None };
//enum class TradeType { Bank, Player, Port, None };
//enum class DevCardType { Knight, VictoryPoint, Monopoly, RoadBuilding, YearOfPlenty, None };
enum class BuildType { Road, Settlement, City, DoubleRoad, None };
enum class ActionType { SwitchToMap, BuildRoad, BuildSettlement, BuildCity, Knight, RoadBuilding, EndTurn, None};
enum ResourceType{
    WOOD,    //forest
    BRICK,    //hill
    SHEEP,    //pasture
    WHEAT,    //field
    ORE,    //mountain
    RESOURCE_COUNT     //desert
};
enum BuildingType{
    ROAD,
    SETTLEMENT,
    CITY,
    BUILDING_COUNT
};
#ifndef EASYX_PROJECT_ENUMCLASS_H
#define EASYX_PROJECT_ENUMCLASS_H

#endif //EASYX_PROJECT_ENUMCLASS_H