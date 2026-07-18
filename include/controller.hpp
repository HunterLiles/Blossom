#include "math.hpp"

#include <GLFW/glfw3.h>

typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

class controller {
private:
  AnimState state = IDLE;
  Direction dir = S;

public:
  controller() = default;
  ~controller() = default;

  float speed = 10.0f, sprint = 1.5f;

  void contr_update(GLFWwindow *window, math::vec2 &pos, float &zoom,
                    float deltaTime) {
    math::vec2 move = {0.0f, 0.0f};

    float currSpeed = speed * deltaTime;
    currSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    ? currSpeed * sprint
                    : currSpeed;
    move.x = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                 ? move.x - currSpeed
                 : move.x;
    move.x = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                 ? move.x + currSpeed
                 : move.x;
    move.y = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                 ? move.y - currSpeed
                 : move.y;
    move.y = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                 ? move.y + currSpeed
                 : move.y;
    pos = pos + move;
    pos = math::clamp(pos, 0.0f, (128 * 32));

    zoom = (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) ? zoom + 0.01f
                                                              : zoom;
    zoom = (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) ? zoom - 0.01f
                                                              : zoom;
    zoom = math::clamp(zoom, 1.0f, 3.0f);

    if (move.x != 0.0f || move.y != 0.0f) {
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        state = RUN;
      else
        state = WALK;

      if (move.x > 0 && move.y > 0)
        dir = SE;
      else if (move.x < 0 && move.y > 0)
        dir = SW;
      else if (move.x > 0 && move.y < 0)
        dir = NE;
      else if (move.x < 0 && move.y < 0)
        dir = NW;
      else if (move.x > 0)
        dir = E;
      else if (move.x < 0)
        dir = W;
      else if (move.y > 0)
        dir = S;
      else
        dir = N;
    } else
      state = IDLE;
  }
};
