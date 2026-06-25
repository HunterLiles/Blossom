#include "raylib.h"
#include "raymath.h"

#include "animation.hpp"
#include "controller.hpp"
#include "gui.hpp"
#include "tilemap.hpp"

int main(void) {
  ChangeDirectory(TextFormat("%s/..", GetApplicationDirectory()));
  InitWindow(1920, 1080, "Blossom");
  gui gui;

  tilemap map;
  auto level =
      map.tilemap_init("first-level"); // TODO : load each level into some
                                       // vector to be used for drawing.
  std::vector<std::string> levelNames = {
      "level one", "level two"}; // TODO : maybe make it to where this is able
                                 // to read the level file names instead?

  // NOTE : Enviornment Init
  Texture2D envTex = LoadTexture("resources/trees.png");
  std::vector<Rectangle> envRec = {{352, 576, 32, 32}, {224, 306, 176, 176}};

  // NOTE : Player Init
  controller player;
  player.pos = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  player.speed = 150.0f;
  player.sprint = 1.5f;
  player.dash = 500.0f;
  player.state = IDLE;
  player.dir = S;
  player.cam = {player.pos, player.pos, 0.0f, 1.0f};

  animation playerAnim;
  playerAnim.counter = 0;
  playerAnim.frame = 0;
  playerAnim.frameSpeed = 8;
  playerAnim.rec = {0, 0, 32, 32};
  playerAnim.tex = {LoadTexture("resources/player-anim/idle.png"),
                    LoadTexture("resources/player-anim/walk.png"),
                    LoadTexture("resources/player-anim/run.png")};

  // NOTE : NPC Init

  SetTargetFPS(180);

  while (!WindowShouldClose()) {

    // NOTE : Pre-rendering things
    playerAnim.anim_update(player.dir, player.state);
    player.pos = Vector2Add(player.pos, player.contr_update(&playerAnim.rec));
    player.cam_update(player);

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(player.cam);

    // NOTE : Background textures
    for (int i = 0; i < 128; i++) {
      for (int j = 0; j < 128; j++) {
        if (level.background[i][j] == 1)
          DrawTextureRec(envTex, envRec[0], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);
      }
    }

    // NOTE : Player and NPC
    DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
                   WHITE);

    // NOTE : Foreground textures
    for (int i = 0; i < 128; i++) {
      for (int j = 0; j < 128; j++) {
        if (level.foreground[i][j] == 2)
          DrawTextureRec(envTex, envRec[1], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);
      }
    }

    // NOTE : Dynamic UI

    EndMode2D();

    // NOTE : Static UI.
    gui.guiSettings(player.pos, levelNames, 0);

    EndDrawing();
  }

  for (int i = 0; i < 3; i++)
    UnloadTexture(playerAnim.tex[i]);
  UnloadTexture(envTex);

  CloseWindow();
  return 0;
}
