#include "controller.h"
#include <raylib.h>

Vector2 contr_update(controller *cont, Rectangle *rec) {
  cont->pos.y = Clamp(cont->pos.y, 0.0f, GetScreenHeight() - rec->height);
  cont->pos.x = Clamp(cont->pos.x, 0.0f, GetScreenWidth() - rec->width);
  Vector2 move = Vector2Zero();

  float currSpeed = cont->speed * GetFrameTime();
  currSpeed =
      (IsKeyDown(KEY_LEFT_SHIFT)) ? currSpeed * cont->sprint : currSpeed;
  move.x = (IsKeyDown(KEY_LEFT)) ? move.x - currSpeed : move.x;
  move.x = (IsKeyDown(KEY_RIGHT)) ? move.x + currSpeed : move.x;
  move.y = (IsKeyDown(KEY_UP)) ? move.y - currSpeed : move.y;
  move.y = (IsKeyDown(KEY_DOWN)) ? move.y + currSpeed : move.y;
  move = IsKeyPressed(KEY_SPACE) ? Vector2Scale(move, cont->dash) : move;

  cont->state = (move.x == 0 && move.y == 0) ? IDLE : cont->state;
  if (move.x > 0) {
    cont->state = WALK;
    cont->dir = E;
  }
  if (move.x < 0) {
    cont->state = WALK;
    cont->dir = W;
  }
  if (move.y > 0) {
    cont->state = WALK;
    cont->dir = S;
  }
  if (move.y < 0) {
    cont->state = WALK;
    cont->dir = N;
  }
  if (move.x > 0 && move.y > 0) {
    cont->state = WALK;
    cont->dir = SE;
  }
  if (move.x < 0 && move.y > 0) {
    cont->state = WALK;
    cont->dir = SW;
  }
  if (move.x > 0 && move.y < 0) {
    cont->state = WALK;
    cont->dir = NE;
  }
  if (move.x < 0 && move.y < 0) {
    cont->state = WALK;
    cont->dir = NW;
  }
  if (IsKeyDown(KEY_LEFT_SHIFT) &&
      ((move.x > 0 || move.y > 0) || (move.x < 0 || move.y < 0)))
    cont->state = RUN;

  return move;
}

void cam_update(Camera2D *cam, controller cont) {
  cam->target = cont.pos;
  cam->zoom = (IsKeyDown(KEY_EQUAL)) ? cam->zoom + 0.01f : cam->zoom;
  cam->zoom = (IsKeyDown(KEY_MINUS)) ? cam->zoom - 0.01f : cam->zoom;
  cam->zoom = (cam->zoom > 3.0f) ? cam->zoom = 3.0f : cam->zoom;
  cam->zoom = (cam->zoom < 1.0f) ? cam->zoom = 1.0f : cam->zoom;

  SetMouseOffset((cam->target.x * cam->zoom) - cam->offset.x,
                 (cam->target.y * cam->zoom) - cam->offset.y);
  SetMouseScale(1.0f / cam->zoom, 1.0f / cam->zoom);
}
