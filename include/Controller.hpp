#pragma once

#include <Core.hpp>
#include <Math.hpp>

typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

class controller {
private:
  AnimState state;
  Direction dir;
  float speed{}, sprint{}, zoom{};

public:
  controller(float speed, float sprint, float zoom)
      : speed(speed), sprint(sprint), zoom(zoom) {}
  ~controller() = default;

  void contr_update(GLFWwindow *window, Math::vec3 &pos, Math::vec3 &target,
                    float deltaTime) {
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

    pos = pos + move;
    target = pos;

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
      zoom += 1.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
      zoom -= 1.0f * deltaTime;

    zoom = Math::clamp(zoom, -5.0f, 5.0f);

    pos.z = zoom;

    state = (move.x == 0 && move.y == 0) ? IDLE : state;
    if (move.x != 0 || move.y != 0) {
      state =
          glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? RUN : WALK;
      if (move.x > 0)
        dir = E;
      if (move.x < 0)
        dir = W;
      if (move.y > 0)
        dir = N;
      if (move.y < 0)
        dir = S;
      if (move.x > 0 && move.y > 0)
        dir = SE;
      if (move.x < 0 && move.y > 0)
        dir = SW;
      if (move.x > 0 && move.y < 0)
        dir = NE;
      if (move.x < 0 && move.y < 0)
        dir = NW;
    }
  }
};
