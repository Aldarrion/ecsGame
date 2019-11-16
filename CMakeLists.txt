cmake_minimum_required(VERSION 3.14)

project(ecsGame)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(SDL_INCLUDE_DIR "" CACHE PATH "Location of the SDL include directory")
set(SDL_LIB_DIR "" CACHE PATH "Location of the SDL libraries")

include_directories(${SDL_INCLUDE_DIR} ${SDL_IMAGE_INCLUDE_DIR})

file(GLOB_RECURSE GameSrc "game/src/*.cpp")
file(GLOB_RECURSE GameHeaders "game/include/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/game/include" PREFIX "game" FILES ${GameHeaders})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/game/src" PREFIX "game" FILES ${GameSrc})

file(GLOB_RECURSE EnttSrc "entt/src/*.cpp")
file(GLOB_RECURSE EnttHeaders "entt/src/*.hpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/entt/src" FILES ${EnttHeaders})

include_directories("entt/src")
include_directories("game/include")

add_executable(ecsGame ${GameSrc} ${GameHeaders} ${EnttHeaders})


if (WIN32)
    target_link_libraries(ecsGame "${SDL_LIB_DIR}/SDL2.lib" "${SDL_LIB_DIR}/SDL2_image.lib" "${SDL_LIB_DIR}/SDL2main.lib")
    
    add_custom_command(TARGET ecsGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/SDL2.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/SDL2_image.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libjpeg-9.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libpng16-16.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libtiff-5.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libwebp-7.dll" ${CMAKE_BINARY_DIR}/Debug
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/zlib1.dll" ${CMAKE_BINARY_DIR}/Debug

        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/SDL2.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/SDL2_image.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libjpeg-9.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libpng16-16.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libtiff-5.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/libwebp-7.dll" ${CMAKE_BINARY_DIR}/Release
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL_LIB_DIR}/zlib1.dll" ${CMAKE_BINARY_DIR}/Release
    )
endif(WIN32)

if (UNIX)
    target_link_libraries(ecsGame "/usr/lib/x86_64-linux-gnu/libSDL2.so" "/usr/lib/x86_64-linux-gnu/libSDL2_image.so")
endif(UNIX)


set_property(TARGET ecsGame PROPERTY CXX_STANDARD 17)
set_property(TARGET ecsGame PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/assets")

