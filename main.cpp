#include "raylib.h"

#include "animation.hpp"
#include "controller.hpp"
#include "gui.hpp"
#include "tilemap.hpp"

int main(void) {
  InitWindow(1920, 1080, "Blossom");
  gui gui;
  tilemap map;

  std::vector<std::string> levelNames = {"one", "two", "three"};
  int currLevel = 0;
  std::vector<tilemap::MapData> level;
  for (int i = 0; i < levelNames.size(); i++) {
    tilemap::MapData levelData = map.init(levelNames[i]);
    level.push_back(levelData);
  }

  // NOTE : Enviornment Init
  Texture2D envTex = LoadTexture("../resources/trees.png");
  Rectangle envRec[10] = {{352, 576, 32, 32}, {224, 306, 176, 176}};

  // NOTE : Player Init
  controller player = {{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
                       150.0f,
                       1.5f,
                       500.0f,
                       IDLE,
                       S,
                       {player.pos, player.pos, 0.0f, 1.0f}};

  animation playerAnim = {{LoadTexture("../resources/player-anim/idle.png"),
                           LoadTexture("../resources/player-anim/walk.png"),
                           LoadTexture("../resources/player-anim/run.png")},
                          {0, 0, 32, 32},
                          0,
                          0,
                          8};

  // NOTE : NPC Init
  animation npcAnim[10] = {{{playerAnim.tex[0]}, {0, 0, 32, 32}, 0, 0, 8}};

  SetTargetFPS(180);

  while (!WindowShouldClose()) {

    // NOTE : Pre-rendering things
    playerAnim.anim_update(player.dir, player.state);
    npcAnim[0].anim_update(S, IDLE);
    player.contr_update(&player.pos, &playerAnim.rec);
    player.cam_update(player);

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(player.cam);

    // NOTE : Background textures
    for (int i = 0; i < 128; i++) {
      for (int j = 0; j < 128; j++) {
        if (level[currLevel].background[i][j] == 1)
          DrawTextureRec(envTex, envRec[0], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);
      }
    }

    // NOTE : Player and NPC
    DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
                   WHITE);
    DrawTextureRec(npcAnim[1].tex[IDLE], npcAnim[1].rec,
                   {10 * 32.0f, 10 * 32.0f}, WHITE);

    // NOTE : Foreground textures
    for (int i = 0; i < 128; i++) {
      for (int j = 0; j < 128; j++) {
        if (level[currLevel].foreground[i][j] == 2)
          DrawTextureRec(envTex, envRec[1], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);
      }
    }

    // NOTE : Dynamic UI

    EndMode2D();

    // NOTE : Static UI.
    rlImGuiBegin();
    gui.guiSettings(player.pos, levelNames, &currLevel);
    rlImGuiEnd();

    EndDrawing();
  }

  for (int i = 0; i < 3; i++) {
    UnloadTexture(playerAnim.tex[i]);
  }
  UnloadTexture(envTex);

  CloseWindow();
  return 0;
}
