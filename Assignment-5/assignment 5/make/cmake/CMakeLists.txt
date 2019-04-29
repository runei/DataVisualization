cmake_minimum_required(VERSION 2.8.7)
set(VTK_DIR /home/leonardo/Documents/VTK/VTK-build/)
project(assignment5)


find_package(VTK REQUIRED)


set(SOURCES ../../source/assignment5.cpp)

add_executable(assignment5 ${SOURCES})
target_link_libraries(assignment5 ${VTK_LIBRARIES})
