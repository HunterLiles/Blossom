#include <raylib.h>
#include <raymath.h>

typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

typedef struct {
  Vector2 pos;
  float speed;
  float sprint;
  float dash;
  AnimState state;
  Direction dir;
} controller;

Vector2 contr_update(controller *cont, Rectangle *rec);
void cam_update(Camera2D *cam, controller cont);
