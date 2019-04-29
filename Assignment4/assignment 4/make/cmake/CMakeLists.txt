cmake_minimum_required(VERSION 2.8.7)
set(VTK_DIR /home/leonardo/Documents/VTK/VTK-build/)
project(assignment4)


find_package(VTK REQUIRED)


set(SOURCES ../../source/assignment4.cpp)

add_executable(assignment4 ${SOURCES})
target_link_libraries(assignment4 ${VTK_LIBRARIES})
