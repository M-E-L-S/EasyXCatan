cmake_minimum_required(VERSION 4.0)
project(EasyX_Project)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(EasyX_Project src/main.cpp
        include/BankPanel.h
        src/BankPanel.cpp
        include/BankLoop.h
        include/HarborPanel.h
        src/HarborPanel.cpp
        include/HarborLoop.h
        include/ImageWindow.h)

target_link_libraries(EasyX_Project libeasyx.a)