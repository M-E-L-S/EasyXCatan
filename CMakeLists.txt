cmake_minimum_required(VERSION 4.0)
project(EasyX_Project)

set(CMAKE_CXX_STANDARD 20)

add_executable(EasyX_Project main.cpp)

target_link_libraries(EasyX_Project libeasyx.a)