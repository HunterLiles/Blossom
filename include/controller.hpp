#include "raylib.h"
#include "raymath.h"

typedef enum { IDLE = 0, WALK, RUN, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;

class controller {
private:
public:
  controller() = default;
  ~controller() = default;

  Vector2 pos;
  float speed;
  float sprint;
  AnimState state;
  Direction dir;
  Camera2D cam;

  void contr_update(Rectangle *rec) {
    pos.y = Clamp(pos.y, 0.0f, (128 * 32) - rec->height);
    pos.x = Clamp(pos.x, 0.0f, (128 * 32) - rec->width);
    Vector2 move = {0.0f, 0.0f};

    float currSpeed = speed * GetFrameTime();
    currSpeed = (IsKeyDown(KEY_LEFT_SHIFT)) ? currSpeed * sprint : currSpeed;
    move.x = (IsKeyDown(KEY_LEFT)) ? move.x - currSpeed : move.x;
    move.x = (IsKeyDown(KEY_RIGHT)) ? move.x + currSpeed : move.x;
    move.y = (IsKeyDown(KEY_UP)) ? move.y - currSpeed : move.y;
    move.y = (IsKeyDown(KEY_DOWN)) ? move.y + currSpeed : move.y;
    pos = pos + move;

    cam.target = pos;
    cam.zoom = (IsKeyDown(KEY_EQUAL)) ? cam.zoom + 0.01f : cam.zoom;
    cam.zoom = (IsKeyDown(KEY_MINUS)) ? cam.zoom - 0.01f : cam.zoom;
    cam.zoom = (cam.zoom > 3.0f) ? cam.zoom = 3.0f : cam.zoom;
    cam.zoom = (cam.zoom < 1.0f) ? cam.zoom = 1.0f : cam.zoom;

    if (move != (Vector2){0.0f, 0.0f}) {
      if (IsKeyDown(KEY_LEFT_SHIFT))
        state = RUN;
      else
        state = WALK;
      if (move.x > 0)
        dir = E;
      if (move.x < 0)
        dir = W;
      if (move.y > 0)
        dir = S;
      if (move.y < 0)
        dir = N;
      if (move.x > 0 && move.y > 0)
        dir = SE;
      if (move.x < 0 && move.y > 0)
        dir = SW;
      if (move.x > 0 && move.y < 0)
        dir = NE;
      if (move.x < 0 && move.y < 0)
        dir = NW;
    } else
      state = IDLE;
  }
};
