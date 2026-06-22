cmake_minimum_required(VERSION 3.15)
project(blossom C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(SOURCES main.c src/controller.c src/animation.c src/tilemap.c src/gui.c)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
