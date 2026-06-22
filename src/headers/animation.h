#include <raylib.h>

typedef struct {
  Texture2D tex[3];
  Rectangle rec;
  int counter;
  int frame;
  int frameSpeed;
} animation;

void anim_update(animation *anim, int dir, int state);
