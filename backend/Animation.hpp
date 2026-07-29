#pragma once

#include <Shapes.hpp>

class Animation {
  typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
  typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

private:
  AnimState state;
  Direction dir;

public:
  Animation() = default;
  ~Animation() = default;

  int counter{}, frame{}, frameSpeed{};

  void anim_update(int width, int height, float deltaTime,
                   Shapes::Rectangle rec) {
    counter++;

    if (counter >= ((1 / deltaTime) / frameSpeed)) {
      counter = 0;
      frame++;

      if (frame >= (width / rec.size.x))
        frame = 0;

      rec.pos.x = ((float)frame * width / ((float)width / rec.size.x));
      rec.pos.y = ((float)dir * height / ((float)height / rec.size.y));
    }
  }

  // ------------------------
  // TODO :
  // This comment section is what needs to be in a player movement script.
  // NOTE :
  //
  // ------------------------

  // state = (move.x == 0 && move.y == 0) ? IDLE : state;
  // if (move.x != 0 || move.y != 0) {
  //   state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? RUN :
  //   WALK; if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  //     dir = E;
  //   if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  //     dir = W;
  //   if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  //   		dir = N;
  //   if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  //     dir = S;
  //   if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  //   &&(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
  //     dir = SE;
  //   if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  //   &&(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
  //     dir = SW;
  //   if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  //   &&(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
  //     dir = NE;
  //   if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  //   &&(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
  //     dir = NW;
  // }
};
