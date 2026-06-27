#include "raylib.h"

class animation {
private:
public:
  animation() = default;
  ~animation() = default;

  Texture2D tex[5];
  Rectangle rec;
  int counter;
  int frame;
  int frameSpeed;

  void anim_update(int dir, int state) {
    counter++;

    if (counter >= ((1 / GetFrameTime()) / frameSpeed)) {
      counter = 0;
      frame++;

      if (frame >= (tex[state].width / rec.width))
        frame = 0;

      rec.x =
          (float)(frame * tex[state].width / (tex[state].width / rec.width));
      rec.y =
          (float)(dir * tex[state].height / (tex[state].height / rec.height));
    }
  }
};
