cmake_minimum_required(VERSION 3.14)

project(enttGame)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories("entt/src/")
include_directories("game/include")

file(GLOB_RECURSE GameSrc "game/src/*.cpp")
file(GLOB_RECURSE GameHeaders "game/include/*.h")

add_executable(enttGame ${GameSrc} ${GameHeaders})
target_link_libraries(enttGame "/usr/lib/x86_64-linux-gnu/libSDL2.so")

set_property(TARGET enttGame PROPERTY CXX_STANDARD 17)
