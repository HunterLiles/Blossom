#pragma once

#define PI 3.14156
#define RAD2DEG (PI / 180)
#define DEG2RAD (180 / PI)

namespace math {
struct vec2 {
  float x, y;
};
struct mat4 {
  float mat[4][4];
};

inline vec2 operator+(vec2 a, vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline vec2 operator+(vec2 v, float n) { return {v.x + n, v.y + n}; }
inline vec2 operator-(vec2 a, vec2 b) { return {a.x - b.x, a.y - b.y}; }
inline vec2 operator-(vec2 v, float n) { return {v.x - n, v.y - n}; }
inline vec2 operator*(vec2 a, vec2 b) { return {a.x * b.x, a.y * b.y}; }
inline vec2 operator*(vec2 v, float n) { return {v.x * n, v.y * n}; }
inline vec2 operator/(vec2 a, vec2 b) { return {a.x / b.x, a.y / b.y}; }

inline vec2 clamp(vec2 v, float min, float max) {
  v = (v.x < min || v.y < min) ? (math::vec2){min, min} : v;
  v = (v.x > max || v.y > max) ? (math::vec2){max, max} : v;
  return v;
}
inline float clamp(float n, float min, float max) {
  n = (n > max) ? max : n;
  n = (n < min) ? min : n;
  return n;
}

} // namespace math
