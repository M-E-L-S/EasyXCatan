cmake_minimum_required(VERSION 4.0)
project(EasyX_Project)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(EasyX_Project src/main.cpp
        src/map.cpp
        src/BankPanel.cpp
        src/DevCardManager.cpp
        src/DevCardPanel.cpp
        src/DevCardShow.cpp
        src/DiscardPanel.cpp
        src/HarborPanel.cpp
        src/Player.cpp
        src/PlayerPanel.cpp
        src/RobbedResourcePanel.cpp
        src/RobberManager.cpp
        src/StartGameUI.cpp
        include/map.h
        include/enumclass.h
        include/BankPanel.h
        include/DevCardManager.h
        include/DevCardPanel.h
        include/DevCardShow.h
        include/DiscardPanel.h
        include/HarborPanel.h
        include/Player.h
        include/PlayerPanel.h
        include/RobbedResourcePanel.h
        include/RobberManager.h
        include/StartGameUI.h
        include/BankLoop.h
        include/DevCardLoop.h
        include/HarborLoop.h
        include/PlayerLoop.h
        include/BGMusic.h
        include/BridgeToPlayer.h
        include/Button.h
        include/Common.h
        include/imageWindow.h
        include/Random.h
)

target_link_libraries(EasyX_Project libeasyx.a easyx winmm MSIMG32)