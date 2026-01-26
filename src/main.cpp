#include "AnimationHandler.hpp"
#include "Controller.hpp"
#include "raylib.h"
#include <vector>

using namespace std;

struct Player {
  Vector2 position;
};

int main(void) {
  const int SCREENWIDTH = 1280;
  const int SCREENHEIGHT = 720;

  InitWindow(SCREENWIDTH, SCREENHEIGHT, "Blossom");

  // --- Camera ---
  Camera2D camera = {{0}};
  camera.offset = {SCREENWIDTH * 0.5f, SCREENHEIGHT * 0.5f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  // --- Environment ---
  vector<Rectangle> environment;

  environment.push_back({SCREENWIDTH * 0.8f, SCREENHEIGHT * 0.5f,
                         SCREENWIDTH * 0.2f, SCREENHEIGHT * 0.01f});
  environment.push_back(
      {0, SCREENHEIGHT * 0.6f, SCREENWIDTH, SCREENHEIGHT * 0.3f});

  // --- Player ---
  Player player;
  player.position = {SCREENWIDTH * 0.4f, SCREENHEIGHT * 0.35f};

  AnimationHandler playerAnimation;
  Controller playerController;

  playerAnimation.AnimInitialize(player.position);
  // --------------

  SetTargetFPS(120);

  while (!WindowShouldClose()) {
    playerAnimation.AnimUpdate(player.position);
    playerController.ContUpdate(player.position, environment, playerAnimation);

    camera.target = player.position;

    BeginDrawing();

    ClearBackground(BLACK);
    BeginMode2D(camera);

    // Player
    playerAnimation.AnimDraw(player.position);

    for (const auto &rect : environment) {
      DrawRectangleLinesEx(rect, 1, RAYWHITE);
    }

    EndMode2D();
    DrawFPS(0, 0);

    DrawText("Test", SCREENWIDTH - 50, 0, 20, MAGENTA);
    EndDrawing();
  }

  playerAnimation.~AnimationHandler();
  playerController.~Controller();

  CloseWindow();

  return 0;
}
