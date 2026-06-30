#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "animation.hpp"
#include "controller.hpp"
#include "gui.hpp"

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 720, "Blossom");

  {
    gui gui(true);
    tilemap map;

    RenderTexture gameTex =
        LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    // NOTE : Tilemap stuff
    std::vector<tilemap::MapData> level;
    for (size_t i{}; i < gui.levels.size(); i++) {
      auto levelData = map.init(gui.levels[i]);
      level.push_back(levelData);
    }

    // NOTE : Enviornment Init
    Texture2D envTex = LoadTexture("../resources/trees.png");
    std::vector<Rectangle> envRec = {
        {}, {352, 576, 32, 32}, {224, 306, 176, 176}};

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

    while (!WindowShouldClose()) {
      // NOTE : Pre-rendering things
      playerAnim.anim_update(player.dir, player.state);
      player.contr_update(&playerAnim.rec);

      BeginDrawing();
      BeginTextureMode(gameTex);
      BeginMode2D(player.cam);
      ClearBackground(BLACK);

      // NOTE : Background textures
      for (size_t i{}; i < tilemap::TILE; i++) {
        for (size_t j{}; j < tilemap::TILE; j++) {
          switch (level[gui.currLevel].background[i][j]) {
          case 0:
            DrawTextureRec(envTex, envRec[0], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          case 1:
            DrawTextureRec(envTex, envRec[1], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          default:
            DrawTextureRec(envTex, envRec[0], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          }
        }
      }

      // NOTE : Player and NPC
      DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
                     WHITE);

      // NOTE : Foreground textures
      for (size_t i{}; i < tilemap::TILE; i++) {
        for (size_t j{}; j < tilemap::TILE; j++) {
          switch (level[gui.currLevel].foreground[i][j]) {
          case 0:
            DrawTextureRec(envTex, envRec[0], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          case 1:
            DrawTextureRec(envTex, envRec[2], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          default:
            DrawTextureRec(envTex, envRec[0], (Vector2){j * 32.0f, i * 32.0f},
                           WHITE);
            break;
          }
        }
      }

      // NOTE : Dynamic UI

      EndTextureMode();
      EndMode2D();

      rlImGuiBegin();
      ImGui::DockSpaceOverViewport(0, NULL,
                                   ImGuiDockNodeFlags_PassthruCentralNode);

      ImGui::Begin("Game Window");
      rlImGuiImageRenderTextureFit(&gameTex, true);
      ImGui::End();

      gui.settings(player.pos);
      gui.tile_editor(&level[gui.currLevel], envTex, envRec);
      gui.log();
      rlImGuiEnd();

      EndDrawing();
    }

    for (size_t i{}; i < 3; i++)
      UnloadTexture(playerAnim.tex[i]);
    UnloadTexture(envTex);
    map.update(&map.map, gui.levels[gui.currLevel]);
  }

  CloseWindow();
  return 0;
}
