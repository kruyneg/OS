cmake_minimum_required(VERSION 3.10)

project(OS_lab2)

set(CMAKE_CXX_STANDARD 17)

add_library(complex_double complex_double.cpp)

add_executable(main2
    main.cpp
    complex_double.cpp)
