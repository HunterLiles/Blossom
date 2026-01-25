#pragma once
#include "AnimationHandler.hpp"
#include "raylib.h"
#include <vector>

using namespace std;

class Controller {
public:
  Vector2 ContUpdate(Vector2 &position, vector<Rectangle> &env,
                     AnimationHandler &anim);

private:
  const float SPEED = 500.0f;
  const float GRAVITY = 1980.0f;
  const float JUMP = -600.0f;
  const float SPRINT = 1.5f;
  // --------------------
  float yVelocity = 0.0f;
  // --------------------
  bool isFlipped = false;
  bool isRunning = false;
  bool isGrounded = false;
  bool isSprinting = false;
  // --------------------
  Texture2D run =
      LoadTexture("/home/hunter/projects/personal/Documents/RaylibProjects/"
                  "NewTest/assets/player run 48x48.png");
  Texture2D idle =
      LoadTexture("/home/hunter/projects/personal/Documents/RaylibProjects/"
                  "NewTest/assets/Player Idle 48x48.png");
  Texture2D jump =
      LoadTexture("/home/hunter/projects/personal/Documents/RaylibProjects/"
                  "NewTest/assets/player new jump 48x48.png");
};
