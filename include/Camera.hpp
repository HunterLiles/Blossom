#include <GLFW/glfw3.h>
#include <Math.hpp>

class camera {
private:
  float f{10.0f}, n{0.01f}, r{0.5f}, t{0.5f}; // Clipping distance

public:
  camera() = default;
  ~camera() = default;

  Math::vec3 eye{0.0f, 0.0f, 3.0f}, center{}, up{0.0f, 1.0f, 0.0f};
  float zoom{1.0f}, speed{1.0f}, sprint{1.5f}, fov{90.0f};

  Math::mat4 view{}, proj{};

  void lookAt() {
    Math::vec3 D = (eye - center).normalize();
    Math::vec3 R = (up.cross(D)).normalize();
    Math::vec3 U = D.cross(R);

    view = {{{R.x, R.y, R.z, -(R.dot(eye))},
             {U.x, U.y, U.z, -(U.dot(eye))},
             {D.x, D.y, D.z, -(D.dot(eye))},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
  }
  void orthographic() {
    proj = {{{2.0f / (r - (-r)), 0.0f, 0.0f, -((r + (-r)) / (r - (-r)))},
             {0.0f, 2.0f / (t - (-t)), 0.0f, -((t + (-t)) / (t - (-t)))},
             {0.0f, 0.0f, -2.0f / (f - n), -((f + n) / (f - n))},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
  }
  void perspective() {
    float scale = 1.0f / tan((fov / 2) * DEG2RAD);
    proj = {{{scale, 0.0f, 0.0f, 0.0f},
             {0.0f, scale, 0.0f, 0.0f},
             {0.0f, 0.0f, -(f / (f - n)), -1.0f},
             {0.0f, 0.0f, -((f * n) / (f - n)), 0.0f}}};
  }

  void Controller(GLFWwindow *window, float deltaTime) {
    Math::vec3 move{};

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      move.x -= (speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      move.x += (speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      move.y += (speed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      move.y -= (speed * deltaTime);

    eye = eye + move;
    center = center + move;
  }
};
