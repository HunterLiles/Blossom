#pragma once

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <Math.hpp>
#include <Window.hpp>

typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

class controller {
private:
  Window input;
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
    currSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    ? currSpeed * sprint
                    : currSpeed;
    move.x = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                 ? move.x - currSpeed
             : (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                 ? move.x + currSpeed
                 : move.x;
    move.y =
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)     ? move.y + currSpeed
        : (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ? move.y - currSpeed
                                                            : move.y;
    pos = pos + move;
    target = target + move;

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
      dir = (move.x > 0)                 ? E
            : (move.x < 0)               ? W
            : (move.y > 0)               ? S
            : (move.y < 0)               ? N
            : (move.x > 0 && move.y > 0) ? SE
            : (move.x < 0 && move.y > 0) ? SW
            : (move.x > 0 && move.y < 0) ? NE
            : (move.x < 0 && move.y < 0) ? NW
                                         : dir;
    }
  }
};
