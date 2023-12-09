cmake_minimum_required(VERSION 3.10)

project(OS_lab5-7 CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)
target_link_libraries(main zmqpp zmq)

add_executable(timer timer.cpp)
target_link_libraries(timer zmqpp zmq)
