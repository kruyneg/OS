cmake_minimum_required(VERSION 3.10)

find_package(Boost REQUIRED)

add_executable(scheduler main.cpp)
# target_link_libraries(Boost)

add_executable(errorjob ~/Programming/OS/course_work/tests/errorjob.cpp)
add_executable(timerjob ~/Programming/OS/course_work/tests/timerjob.cpp)
