class animation {
private:
  typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
  typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

  int counter{};
  int frame{};

public:
  animation() = default;
  ~animation() = default;

  int frameSpeed{};

  AnimState state = IDLE;
  Direction dir = S;

  void anim_update(float deltaTime) {
    // counter++;
    //
    // if (counter >= ((1 / deltaTime) / frameSpeed)) {
    //   counter = 0;
    //   frame++;
    //
    //   if (frame >= (tex[state].width / rec.width))
    //     frame = 0;
    //
    //   rec.x =
    //       (float)(frame * tex[state].width / (tex[state].width / rec.width));
    //   rec.y =
    //       (float)(dir * tex[state].height / (tex[state].height /
    //       rec.height));
    // }
  }

  // if (move.x != 0.0f || move.y != 0.0f) {
  //   if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  //     state = RUN;
  //   else
  //     state = WALK;
  //
  //   if (move.x > 0 && move.y > 0)
  //     dir = SE;
  //   else if (move.x < 0 && move.y > 0)
  //     dir = SW;
  //   else if (move.x > 0 && move.y < 0)
  //     dir = NE;
  //   else if (move.x < 0 && move.y < 0)
  //     dir = NW;
  //   else if (move.x > 0)
  //     dir = E;
  //   else if (move.x < 0)
  //     dir = W;
  //   else if (move.y > 0)
  //     dir = S;
  //   else
  //     dir = N;
  // } else
  //   state = IDLE;
};
