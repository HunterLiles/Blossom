#pragma once

#include <Math.hpp>

class camera {
private:
  float f = 10.0f, n = -1.0f;
  float r = 0.5f, t = 0.5f;

public:
  camera() = default;
  ~camera() = default;

  Math::vec3 center{}, eye{0.0f, 0.0f, 1.0f}, up{0.0f, 1.0f, 0.0f};

  Math::mat4 viewMat() {
    Math::vec3 forward = (center - eye).normalize();
    Math::vec3 U = (forward.cross(up)).normalize();
    Math::vec3 V = U.cross(forward);
    Math::vec3 W = -forward;

    return {{{{U.x, U.y, U.z, -eye.dot(U)},
              {V.x, V.y, V.z, -eye.dot(V)},
              {W.x, W.y, W.z, -eye.dot(W)},
              {0.0f, 0.0f, 0.0f, 1.0f}}}};
  }
  Math::mat4 projMat() {
    return {{{{1 / r, 0, 0, 0},
              {0, 1 / t, 0, 0},
              {0, 0, (-2) / (f - n), -((f + n) / (f - n))},
              {0, 0, 0, 1}}}};
  }
};
