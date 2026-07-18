#include "math.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

class camera {
private:
  float F = 1.0f,
        N = -1.0f; // Clipping distance of the camera (camera distance)

public:
  camera() = default;
  ~camera() = default;

  math::vec2 pos, target;
  float zoom;

  void looAt(math::mat4 &mat) {
    math::vec2 direction = pos - target;
    float theta = atan2(direction.y, direction.x);

    mat.mat[0][0] = (zoom * (float)cos(theta));
    mat.mat[0][1] = -(zoom * (float)sin(theta));
    mat.mat[0][3] =
        zoom * -((float)cos(theta) * pos.x + (float)sin(theta) * pos.y);
    mat.mat[1][0] = (zoom * (float)sin(theta));
    mat.mat[1][1] = (zoom * (float)cos(theta));
    mat.mat[1][3] =
        zoom * -(-(float)sin(theta) * pos.x + (float)cos(theta) * pos.y);
    mat.mat[2][2] = 1.0f;
    mat.mat[3][3] = 1.0f;
  }
  void projection(GLFWwindow *window, math::mat4 &mat) {
    float R = 0.5f, L = -0.5f, T = 0.5f, B = -0.5f;

    mat.mat[0][0] = (2.0f / (R - L));
    mat.mat[0][3] = -((float)(R + L) / (R - L));
    mat.mat[1][1] = (2.0f / (T - B));
    mat.mat[1][3] = -((float)(T + B) / (T - B));
    mat.mat[2][2] = (-2.0f / (F - N));
    mat.mat[2][3] = -((float)(F + N) / (F - N));
    mat.mat[3][3] = 1.0f;
  }
};
