cmake_minimum_required(VERSION 3.15)
project(blossom)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    raygui
    URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
)
FetchContent_Populate(raygui)

add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE ${raygui_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib raygui)
