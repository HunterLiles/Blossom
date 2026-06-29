#include "raylib.h"
#include "rlImGui.h"

#include "animation.hpp"
#include "controller.hpp"
#include "gui.hpp"
#include "tilemap.hpp"

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(1920, 1080, "Blossom");

  gui gui(true);
  tilemap map;

  // NOTE : Tilemap stuff
  std::vector<std::string> levelNames = {"one", "two", "three"};
  int currLevel = 0;
  std::vector<tilemap::MapData> level;
  for (int i = 0; i < levelNames.size(); i++) {
    auto levelData = map.init(levelNames[i]);
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
    for (size_t i{}; i < TILE; i++)
      for (size_t j{}; j < TILE; j++)
        if (level[currLevel].background[i][j] == 1)
          DrawTextureRec(envTex, envRec[0], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);

    // NOTE : Player and NPC
    DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
                   WHITE);
    DrawTextureRec(npcAnim[0].tex[IDLE], npcAnim[0].rec,
                   {10 * 32.0f, 10 * 32.0f}, WHITE);

    // NOTE : Foreground textures
    for (size_t i{}; i < TILE; i++)
      for (size_t j{}; j < TILE; j++)
        if (level[currLevel].foreground[i][j] == 1)
          // FIX : How do I move the texture by width / 2 and -height?
          DrawTextureRec(envTex, envRec[1], (Vector2){i * 32.0f, j * 32.0f},
                         WHITE);

    // NOTE : Dynamic UI

    EndMode2D();

    // NOTE : Static UI.
    rlImGuiBegin();
    gui.settings(player.pos, levelNames, &currLevel);
    rlImGuiEnd();

    EndDrawing();
  }

  for (size_t i{}; i < 3; i++)
    UnloadTexture(playerAnim.tex[i]);
  UnloadTexture(envTex);

  gui.~gui();
  CloseWindow();
  return 0;
}
