cmake_minimum_required(VERSION 3.10)

project(OS_lab4 CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(simple SHARED simple.cpp)
add_library(difficult SHARED difficult.cpp)

add_executable(first main1.cpp)
target_link_libraries(first simple)

add_executable(second main2.cpp)
