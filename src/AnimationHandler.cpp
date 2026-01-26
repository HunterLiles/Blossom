#include "AnimationHandler.hpp"

void AnimationHandler::AnimInitialize(Vector2 position) {
  frameWidth = ((float)currTexture.width / NUM_FRAMES_PER_LINE);
  frameHeight = ((float)currTexture.height / NUM_LINES);
  frameRec = {position.x, position.y, frameWidth, frameHeight};
}

AnimationHandler::~AnimationHandler() { UnloadTexture(currTexture); }

void AnimationHandler::AnimUpdate(Vector2 &position) {

  frameCounter++;
  if (frameCounter > 10) {
    currentFrame++;
    if (currentFrame >= NUM_FRAMES_PER_LINE) {
      currentFrame = 0;
      currentLine++;

      if (currentLine >= NUM_LINES) {
        currentLine = 0;
      }
    }
    frameCounter = 0;
  }

  frameRec.x = frameWidth * currentFrame;
  frameRec.y = frameHeight * currentLine;
}

void AnimationHandler::AnimDraw(Vector2 position) {
  DrawTextureRec(currTexture, frameRec, position, WHITE);
}

Texture2D AnimationHandler::FlipAnimation() {
  frameRec.width = -frameRec.width;
  return currTexture;
}
Texture2D AnimationHandler::SetTexture(Texture2D newTexture) {
  return currTexture = newTexture;
}

Rectangle AnimationHandler::GetRect(Vector2 position) {
  return Rectangle{position.x, position.y, frameWidth, frameHeight};
}
