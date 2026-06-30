#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "tilemap.hpp"

#include <string>
#include <vector>

class gui {
private:
  std::vector<std::string> texID = {"Air {0}", "Grass {1}", "Tree {2}"};
  int currTex = 0;

  RenderTexture2D tex =
      LoadRenderTexture(tilemap::TILE * 32.0f, tilemap::TILE * 32.0f);

public:
  gui(bool DarkMode) {
    rlImGuiSetup(DarkMode);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  ~gui() { rlImGuiShutdown(); };

  std::vector<std::string> levels = {"one", "two", "three"};
  int currLevel = 0;

  void settings(Vector2 pos) {
    ImGui::Begin("Settings");
    ImGui::Text("FPS : %.2d", GetFPS());
    ImGui::Text("X : %.2f | Y : %.2f", pos.x, pos.y);

    if (ImGui::BeginCombo("Level", levels[currLevel].c_str())) {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                          ImVec2(0.5f, 0.5f));

      for (size_t n{}; n < levels.size(); n++) {
        bool isSelected = (currLevel == n);
        if (ImGui::Selectable(levels[n].c_str(), isSelected))
          currLevel = n;

        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::PopStyleVar();
      ImGui::EndCombo();
    }

    ImGui::End();
  }

  void tile_editor(tilemap::MapData *map, Texture2D envTex,
                   std::vector<Rectangle> envRec) {
    ImGui::Begin("Level Editor");

    if (ImGui::BeginCombo("TextureID", texID[currTex].c_str())) {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                          ImVec2(0.5f, 0.5f));

      for (size_t i{}; i < texID.size(); i++) {
        bool isSelected = (currTex == i);
        if (ImGui::Selectable(texID[i].c_str(), isSelected))
          currTex = i;
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::PopStyleVar();
      ImGui::EndCombo();
    }

    Vector2 mouse = {GetMousePosition().x - ImGui::GetCursorScreenPos().x,
                     GetMousePosition().y - ImGui::GetCursorScreenPos().y};
    Vector2 collisionPos;

    BeginTextureMode(tex);
    ClearBackground(BLACK);
    for (size_t i{}; i < tilemap::TILE; i++) {
      for (size_t j{}; j < tilemap::TILE; j++) {
        Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};

        // ------------ Finding texture ID ------------ //
        switch (map->background[i][j]) {
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
        // --------------------------------------------- //

        // ----------- Collision ------------- //
        if (CheckCollisionPointRec(mouse, gridRec))
          DrawRectangleLinesEx(gridRec, 1.5f, WHITE);

        if (CheckCollisionPointRec(mouse, gridRec) &&
            IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          collisionPos = {(float)j, (float)i};
          map->background[(int)collisionPos.y][(int)collisionPos.x] = currTex;
        }
        // ------------------------------------ //
      }
    }
    EndTextureMode();

    rlImGuiImageRenderTexture(&tex);
    ImGui::End();
  }

  void log() {
    ImGui::Begin("Log");
    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Test");
    ImGui::End();
  }
};
