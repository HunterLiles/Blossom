#pragma once

#include <Core.hpp>
#include <Math.hpp>

class Camera {
private:
  float f = 10.0f, n = -1.0f;
  float r = 0.5f, t = 0.5f;
  Math::mat4 view = {.mat{}}, proj = {.mat{}};

public:
  Camera() = default;
  ~Camera() = default;

  Math::vec3 center{}, eye{0.0f, 0.0f, 5.0f}, up{0.0f, 1.0f, 0.0f};
  float speed{1.0f}, sprint{1.5f}, zoom{1.0f};

  // ---------------------
  // TODO :
  // How do I add in FOV?
  // NOTE :
  //
  // ---------------------

  void Update(unsigned int ID) {
    view = viewMat();
    proj = projMat();

    GLint viewLoc = glGetUniformLocation(ID, "view");
    GLint projLoc = glGetUniformLocation(ID, "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_TRUE,
                       reinterpret_cast<const GLfloat *>(view.mat.data()));
    glUniformMatrix4fv(projLoc, 1, GL_TRUE,
                       reinterpret_cast<const GLfloat *>(proj.mat.data()));
  }

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

  void Controller(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    Math::vec3 move{};

    float currSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      currSpeed *= sprint;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      move.x -= currSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      move.x += currSpeed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      move.y += currSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      move.y -= currSpeed;

    eye = eye + move;
    center = center + move;

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
      zoom += 1.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
      zoom -= 1.0f * deltaTime;

    zoom = Math::clamp(zoom, -5.0f, 5.0f);

    eye.z = zoom;
  }
};
