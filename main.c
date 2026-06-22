#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include "animation.h"
#include "controller.h"
#include "gui.h"
#include "tilemap.h"

int main(void) {
  ChangeDirectory(TextFormat("%s/..", GetApplicationDirectory()));
  InitWindow(1920, 1080, "Blossom");

  Rectangle grid = {0, 0, 32, 32}; // temp

  // NOTE : Enviornment Init
  Texture2D envTex = LoadTexture("assets/trees.png");
  Rectangle envGRec = {336, 560, 64, 64};
  Rectangle envSGRec = {352, 576, 32, 32};
  Rectangle envTRec = {224, 306, 176, 176};
  Vector2 envPos = {500, 500};

  // NOTE : Player Init
  Texture2D playerTex[3] = {LoadTexture("assets/player-anim/idle.png"),
                            LoadTexture("assets/player-anim/walk.png"),
                            LoadTexture("assets/player-anim/run.png")};
  controller player = {
      .pos = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
      .speed = 150.0f,
      .sprint = 1.5f,
      .dir = S,
      .state = IDLE,
      .dash = 500.0f};
  animation playerAnim = {
      .counter = 0,
      .frame = 0,
      .frameSpeed = 8,
      .rec = {0, 0, 32, 32},
      .tex = {playerTex[0], playerTex[1], playerTex[2]},
  };
  Camera2D cam = {player.pos, player.pos, 0.0f, 1.0f};

  // NOTE : NPC Init

  SetTargetFPS(180);

  while (!WindowShouldClose()) {

    // NOTE : Pre-rendering things
    anim_update(&playerAnim, player.dir, player.state);
    player.pos = Vector2Add(player.pos, contr_update(&player, &playerAnim.rec));
    cam_update(&cam, player);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      CheckCollisionPointRec(GetMousePosition(), grid);

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(cam);

    // NOTE : Background textures
    DrawTextureRec(envTex, envGRec,
                   Vector2Subtract(
                       envPos, (Vector2){envGRec.width / 2.0f, envGRec.height}),
                   WHITE);

    // NOTE : Player and NPC
    DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
                   WHITE);

    // NOTE : Foreground textures
    DrawTextureRec(envTex, envTRec,
                   Vector2Subtract(envPos, (Vector2){envTRec.width / 2.0f,
                                                     envTRec.height + 25}),
                   WHITE);

    // NOTE : Dynamic UI
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 64; j++) {
        grid.x = i * 32;
        grid.y = j * 32;
        if (CheckCollisionPointRec(GetMousePosition(), grid)) {
          DrawRectangleLinesEx(grid, 1.5f, WHITE);
        } else
          DrawRectangleLinesEx(grid, 1.0f, DARKGRAY);
      }
    }

    EndMode2D();

    // NOTE : Static UI.
    guiSettings(player.pos);

    EndDrawing();
  }

  for (int i = 0; i < 3; i++)
    UnloadTexture(playerTex[i]);
  UnloadTexture(envTex);

  CloseWindow();
  return 0;
}
