# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.5.1)
# Project's name
project(lane_detection)
# Set the output folder where your program will be created
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenCV REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

include(CMakeToolsHelpers OPTIONAL)
include_directories(include ${PROJECT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

#sources
file(GLOB SOURCES "src/*.cpp")

add_library(lane ${SOURCES})
target_link_libraries(lane ${OpenCV_LIBS})

add_executable(lane_detect ${SOURCES}) #lane_detect.cpp
#add_executable(test1 test1.cpp)
target_link_libraries(lane_detect lane ${OpenCV_LIBS})
#target_link_libraries(test1 ${OpenCV_LIBS})
# The following folder will be included
#include_directories("${PROJECT_SOURCE_DIR}")
