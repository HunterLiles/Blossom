#include "Controller.hpp"

Vector2 Controller::ContUpdate(Vector2 &position, vector<Rectangle> &env,
                               AnimationHandler &anim) {
  float dt = GetFrameTime();
  float move = 0.0f;

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
    move--;
    if (!isFlipped) {
      anim.FlipAnimation();
      isFlipped = true;
    }
  }

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
    move++;
    if (isFlipped) {
      anim.FlipAnimation();
      isFlipped = false;
    }
  }

  if (IsKeyDown(KEY_LEFT_SHIFT))
    isSprinting = true;
  else
    isSprinting = false;

  float finalMove = move * SPEED * dt;
  position.x += isSprinting ? finalMove * SPRINT : finalMove;

  Rectangle playerRect = anim.GetRect(position);
  for (const auto &obstacle : env) {
    if (CheckCollisionRecs(playerRect, obstacle)) {
      if (move > 0) {
        position.x = obstacle.x - playerRect.width;
      }
      if (move < 0) {
        position.x = obstacle.x + obstacle.width;
      }
    }
  }
  yVelocity += GRAVITY * dt;

  if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) ||
       IsKeyPressed(KEY_UP)) &&
      isGrounded) {
    yVelocity = JUMP;
    isGrounded = false;
  }
  position.y += yVelocity * dt;

  isGrounded = false;
  playerRect = anim.GetRect(position);

  for (const auto &obstacle : env) {
    if (CheckCollisionRecs(playerRect, obstacle)) {
      if (yVelocity > 0) {
        position.y = obstacle.y - playerRect.height;
        yVelocity = 0;
        isGrounded = true;
      }
      if (yVelocity < 0) {
        position.y = obstacle.y + obstacle.height;
        yVelocity = 0;
      }
    }
  }

  if (isGrounded) {
    if (move != 0.0f)
      anim.SetTexture(run);
    else
      anim.SetTexture(idle);
  } else {
    anim.SetTexture(jump);
  }

  return position;
}
