cmake_minimum_required(VERSION 3.14...4.3)
project(blossom VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking)

FetchContent_MakeAvailable(imgui)

set(IMGUI
    ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp)

add_executable(${PROJECT_NAME} src/main.cpp src/gl.c ${IMGUI})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)
