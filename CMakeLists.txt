cmake_minimum_required(VERSION 3.14...4.3)
project(blossom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(include/thirdparty/raylib)

include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking)
FetchContent_Declare(
  rlimgui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG Raylib_6_0)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(rlimgui)

set(IMGUI
    ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI}
                               ${rlimgui_SOURCE_DIR}/rlImGui.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${rlimgui_SOURCE_DIR} ${imgui_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)
