#pragma once
#include "raylib.h"

#define NUM_FRAMES_PER_LINE 10
#define NUM_LINES 1

class AnimationHandler {
public:
  ~AnimationHandler();
  void AnimInitialize(Vector2 position);
  void AnimDraw(Vector2 position);
  void AnimUpdate(Vector2 &positon);
  // --------------------
  Texture2D FlipAnimation();
  Texture2D SetTexture(Texture2D newTexture);
  // --------------------
  Rectangle GetRect(Vector2 position);

private:
  Texture2D currTexture =
      LoadTexture("/home/hunter/projects/personal/Documents/RaylibProjects/"
                  "NewTest/assets/Player Idle 48x48.png");
  // --------------------
  Rectangle frameRec;
  // --------------------
  float frameWidth = 0;
  float frameHeight = 0;
  int frameCounter = 0;
  // --------------------
  int currentFrame = 0;
  int currentLine = 0;
  // --------------------
};
