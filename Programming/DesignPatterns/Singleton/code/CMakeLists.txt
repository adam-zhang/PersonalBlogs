cmake_minimum_required(VERSION 2.8)
project(singleton)
add_executable(singleton main.cpp)
target_link_libraries(singleton pthread)

