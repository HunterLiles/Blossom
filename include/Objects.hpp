#pragma once

#include "Math.hpp"

#include <cstdint>
#include <string>
#include <vector>

// Script attachments are data owned by scene objects. A project build can later
// resolve the source path into a native behavior without changing the object API.
struct ScriptAttachment {
  std::string source;
  bool enabled = true;
};

// Scene primitives stay as plain data. Add future mesh/object primitives here.
struct Cube {
  math::Vec3 position{};
  float angle = 0.0f;
  std::vector<ScriptAttachment> scripts;
};

inline constexpr uint32_t cubeVertexCount = 36;
