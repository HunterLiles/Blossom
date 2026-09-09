#pragma once

#include "Math.hpp"

#include <cstdint>

// Scene primitives stay as plain data. Add future mesh/object primitives here.
struct Cube {
  math::Vec3 position{};
  float angle = 0.0f;
};

inline constexpr uint32_t cubeVertexCount = 36;
