#pragma once

#include <cmath>

#define PI 3.14159265
#define RAD2DEG (PI / 180)
#define DEG2RAD (180 / PI)

namespace Math {
struct vec2 {
  float x, y;

  vec2 operator+(vec2 b) { return {x + b.x, y + b.y}; }
  vec2 operator+(float n) { return {x + n, y + n}; }
  vec2 operator-(vec2 b) { return {x - b.x, y - b.y}; }
  vec2 operator-(float n) { return {x - n, y - n}; }
  vec2 operator-() { return {-x, -y}; }
  vec2 operator*(vec2 b) { return {x * b.x, y * b.y}; }
  vec2 operator*(float n) { return {x * n, y * n}; }
  vec2 operator/(vec2 b) { return {x / b.x, y / b.y}; }

  vec2 clamp(float min, float max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    return *this;
  }

  float length() { return sqrt(x * x + y * y); }
  vec2 normalize() {
    float len = length();
    return {x / len, y / len};
  }

  float dot(vec2 b) { return x * b.x + y * b.y; }
};
struct vec3 {
  float x, y, z;

  vec3 operator+(vec3 b) { return {x + b.x, y + b.y, z + b.z}; }
  vec3 operator+(float n) { return {x + n, y + n, z + n}; }
  vec3 operator-(vec3 b) { return {x - b.x, y - b.y, z - b.z}; }
  vec3 operator-(float n) { return {x - n, y - n, z - n}; }
  vec3 operator-() { return {-x, -y, -z}; }
  vec3 operator*(vec3 b) { return {x * b.x, y * b.y, z * b.z}; }
  vec3 operator*(float n) { return {x * n, y * n, z * n}; }
  vec3 operator/(vec3 b) { return {x / b.x, y / b.y, z / b.z}; }

  vec3 clamp(float min, float max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    z = (z < min) ? min : (z > max) ? max : z;
    return *this;
  }

  float length() { return sqrt(x * x + y * y + z * z); }
  vec3 normalize() {
    float len = length();
    return {x / len, y / len, z / len};
  }

  float dot(vec3 b) { return x * b.x + y * b.y + z * b.z; }
  vec3 cross(vec3 b) {
    return {(y * b.z - z * b.y), (z * b.x - x * b.z), (x * b.y - y * b.x)};
  }
};
struct vec4 {
  float x, y, z, w;

  vec4 operator+(vec4 b) { return {x + b.x, y + b.y, z + b.z, w + b.w}; }
  vec4 operator+(float n) { return {x + n, y + n, z + n, w + n}; }
  vec4 operator-(vec4 b) { return {x - b.x, y - b.y, z - b.z, w - b.w}; }
  vec4 operator-(float n) { return {x - n, y - n, z - n, w - n}; }
  vec4 operator-() { return {-x, -y, -z, -w}; }
  vec4 operator*(vec4 b) { return {x * b.x, y * b.y, z * b.z, w * b.w}; }
  vec4 operator*(float n) { return {x * n, y * n, z * n, w * n}; }
  vec4 operator/(vec4 b) { return {x / b.x, y / b.y, z / b.z, w / b.w}; }

  vec4 clamp(float min, float max) {
    x = (x < min) ? min : (x > max) ? max : x;
    y = (y < min) ? min : (y > max) ? max : y;
    z = (z < min) ? min : (z > max) ? max : z;
    w = (w < min) ? min : (w > max) ? max : w;
    return *this;
  }

  float length() { return sqrt(x * x + y * y + z * z + w * w); }
  vec4 normalize() {
    float len = length();
    return {x / len, y / len, z / len, w / len};
  }

  float dot(vec4 b) { return x * b.x + y * b.y + z * b.z + w * b.w; }
};
struct mat4 {
  vec4 mat[4];
};

inline float clamp(float n, float min, float max) {
  n = (n > max) ? max : n;
  n = (n < min) ? min : n;
  return n;
}
} // namespace Math
