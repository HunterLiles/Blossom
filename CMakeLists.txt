cmake_minimum_required(VERSION 3.20)
project(blossom_vulkan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_program(GLSLC glslc REQUIRED)
include(cmake/ImGui.cmake)

set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
set(SHADERS
  "${SHADER_OUTPUT_DIR}/cube.vert.spv"
  "${SHADER_OUTPUT_DIR}/cube.frag.spv"
  "${SHADER_OUTPUT_DIR}/grid.vert.spv"
  "${SHADER_OUTPUT_DIR}/grid.frag.spv"
)
add_custom_command(
  OUTPUT ${SHADERS}
  COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIR}"
  COMMAND ${GLSLC} -fshader-stage=vert --target-env=vulkan1.0 "${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.vert.hlsl" -o "${SHADER_OUTPUT_DIR}/cube.vert.spv"
  COMMAND ${GLSLC} -fshader-stage=frag --target-env=vulkan1.0 "${CMAKE_CURRENT_SOURCE_DIR}/shaders/cube.frag.hlsl" -o "${SHADER_OUTPUT_DIR}/cube.frag.spv"
  COMMAND ${GLSLC} -fshader-stage=vert --target-env=vulkan1.0 "${CMAKE_CURRENT_SOURCE_DIR}/shaders/grid.vert.hlsl" -o "${SHADER_OUTPUT_DIR}/grid.vert.spv"
  COMMAND ${GLSLC} -fshader-stage=frag --target-env=vulkan1.0 "${CMAKE_CURRENT_SOURCE_DIR}/shaders/grid.frag.hlsl" -o "${SHADER_OUTPUT_DIR}/grid.frag.spv"
  DEPENDS shaders/cube.vert.hlsl shaders/cube.frag.hlsl shaders/grid.vert.hlsl shaders/grid.frag.hlsl
  VERBATIM
)
add_custom_target(shaders DEPENDS ${SHADERS})

add_executable(blossom_vulkan main.cpp)
add_dependencies(blossom_vulkan shaders)
target_include_directories(blossom_vulkan PRIVATE include)
target_link_libraries(blossom_vulkan PRIVATE Vulkan::Vulkan glfw imgui)
target_compile_definitions(blossom_vulkan PRIVATE
  SHADER_DIRECTORY="${SHADER_OUTPUT_DIR}"
  ENGINE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}"
  NEOVIM_CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/editor/nvim/init.lua"
)
