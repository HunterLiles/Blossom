#pragma once

class Animation {
private:
public:
  Animation() = default;
  ~Animation() = default;

  int counter{}, frame{}, frameSpeed{};

  void anim_update(int dir, int state, float deltaTime) {
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
    //       }
  }
};
