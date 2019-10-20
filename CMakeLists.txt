cmake_minimum_required(VERSION 3.14)

project(enttGame)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(enttGame game/src/main.cpp)
target_link_libraries(enttGame ${SDL2_LIBRARIES})

