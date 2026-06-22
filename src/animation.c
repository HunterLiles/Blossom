#include "animation.h"

void anim_update(animation *anim, int dir, int state) {
  anim->counter++;

  if (anim->counter >= ((1 / GetFrameTime()) / anim->frameSpeed)) {
    anim->counter = 0;
    anim->frame++;

    if (anim->frame >= (anim->tex[state].width / anim->rec.width))
      anim->frame = 0;

    anim->rec.x = (float)(anim->frame * anim->tex[state].width /
                          (anim->tex[state].width / anim->rec.width));
    anim->rec.y = (float)(dir * anim->tex[state].height /
                          (anim->tex[state].height / anim->rec.height));
  }
}
