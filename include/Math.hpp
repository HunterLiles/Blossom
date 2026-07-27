#pragma once

#include <array>
#include <cmath>

namespace Math {
struct vec2 {
  float x, y;

  vec2 operator+(const vec2 &b) const { return {x + b.x, y + b.y}; }
  vec2 operator+(const float &n) const { return {x + n, y + n}; }
  vec2 operator-(const vec2 &b) const { return {x - b.x, y - b.y}; }
  vec2 operator-(const float &n) const { return {x - n, y - n}; }
  vec2 operator-() const { return {-x, -y}; }
  vec2 operator*(const vec2 &b) const { return {(x * b.x), (y * b.y)}; }
  vec2 operator*(const float &n) const { return {x * n, y * n}; }

  vec2 xy() const { return {x, y}; }
  vec2 &clamp(const float &min, const float &max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    return *this;
  }

  float dot(const vec2 &b) const { return (x * b.x) + (y * b.y); };
  float length() const { return std::sqrt(x * x + y * y); }
  vec2 normalize() const {
    float len = length();
    if (len > 0.0f)
      return {(x / len), (y / len)};
    return {};
  }
};

struct vec3 {
  float x, y, z;
  vec3 operator+(const vec3 &b) const { return {x + b.x, y + b.y, z + b.z}; }
  vec3 operator+(const float &n) const { return {x + n, y + n, z + n}; }
  vec3 operator-(const vec3 &b) const { return {x - b.x, y - b.y, z - b.z}; }
  vec3 operator-(const float &n) const { return {x - n, y - n, z - n}; }
  vec3 operator-() const { return {-x, -y, -z}; }
  vec3 operator*(const vec3 &b) const {
    return {(x * b.x), (y * b.y), (z * b.z)};
  }
  vec3 operator*(const float &n) const { return {x * n, y * n, z * n}; }

  vec2 xy() const { return {x, y}; }
  vec3 xyz() const { return {x, y, z}; }
  vec3 &clamp(const float &min, const float &max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    z = (z < min) ? min : (z > max) ? max : z;
    return *this;
  }
  float dot(const vec3 &b) const { return (x * b.x) + (y * b.y) + (z * b.z); };
  vec3 cross(const vec3 &b) const {
    return {(y * b.z) - (z * b.y), (z * b.x) - (x * b.z),
            (x * b.y) - (y * b.x)};
  }
  float length() const { return std::sqrt(x * x + y * y + z * z); }
  vec3 normalize() const {
    float len = length();
    if (len > 0.0f)
      return {(x / len), (y / len), (z / len)};
    return {};
  }
};

struct vec4 {
  float x, y, z, w;

  vec4 operator+(const vec4 &b) const {
    return {x + b.x, y + b.y, z + b.z, w + b.w};
  }
  vec4 operator+(const float &n) const { return {x + n, y + n, z + n, w + n}; }
  vec4 operator-(const vec4 &b) const {
    return {x - b.x, y - b.y, z - b.z, w - b.w};
  }
  vec4 operator-(const float &n) const { return {x - n, y - n, z - n, w - n}; }
  vec4 operator-() const { return {-x, -y, -z, -w}; }
  vec4 operator*(const vec4 &b) const {
    return {(x * b.x), (y * b.y), (z * b.z), (w * b.w)};
  }
  vec4 operator*(const float &n) const { return {x * n, y * n, z * n, w * n}; }

  vec2 xy() const { return {x, y}; }
  vec3 xyz() const { return {x, y, z}; }
  vec4 xyzw() const { return {x, y, z, w}; }
  vec4 &clamp(const float &min, const float &max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    z = (z < min) ? min : (z > max) ? max : z;
    w = (w < min) ? min : (w > max) ? max : w;
    return *this;
  }

  float dot(const vec4 &b) const {
    return (x * b.x) + (y * b.y) + (z * b.z) + (w * b.w);
  };
  float length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
  vec4 normalize() const {
    float len = length();
    if (len > 0.0f)
      return {(x / len), (y / len), (z / len), (w / len)};
    return {};
  }
};

struct mat4 {
  std::array<vec4, 4> mat;

  mat4 operator+(const mat4 &B) const {
    return {{mat[0] + B.mat[0], mat[1] + B.mat[1], mat[2] + B.mat[2],
             mat[3] + B.mat[3]}};
  }
  mat4 operator+(const float &n) const {
    return {{mat[0] + n, mat[1] + n, mat[2] + n, mat[3] + n}};
  }
  mat4 operator*(const mat4 &B) const {
    mat4 result{};
    for (int i{}; i < 4; i++) {
      result.mat[i] = (B.mat[0] * mat[i].x) + (B.mat[1] * mat[i].y) +
                      (B.mat[2] * mat[i].z) + (B.mat[3] * mat[i].w);
    }
    return result;
  }
  vec4 operator*(const vec4 &v) const {
    return {mat[0].dot(v), mat[1].dot(v), mat[2].dot(v), mat[3].dot(v)};
  }
  mat4 operator*(const float &n) const {
    return {mat[0] * n, mat[1] * n, mat[2] * n, mat[3] * n};
  }
};

inline float clamp(float &n, float min, float max) {
  return n = (n < min) ? min : (n > max) ? max : n;
}
} // namespace Math
