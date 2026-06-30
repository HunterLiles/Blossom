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

  RenderTexture2D tex = LoadRenderTexture(128 * 32.0f, 128 * 32.0f);

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

  void tile_editor(tilemap::MapData map, Texture2D envTex,
                   Rectangle envRec[10]) {
    ImGui::Begin("Level Editor");
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    Vector2 mouse = {GetMousePosition().x - screenPos.x,
                     GetMousePosition().y - screenPos.y};

    BeginTextureMode(tex);
    ClearBackground(BLACK);
    for (size_t i{}; i < 128; i++) {
      for (size_t j{}; j < 128; j++) {
        Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};
        if (map.background[i][j] == 1)
          DrawTextureRec(envTex, envRec[0], (Vector2){j * 32.0f, i * 32.0f},
                         WHITE);
        if (map.foreground[i][j] == 1)
          DrawTextureRec(envTex, envRec[1], (Vector2){j * 32.0f, i * 32.0f},
                         WHITE);
        if (CheckCollisionPointRec(mouse, gridRec))
          DrawRectangleLinesEx(gridRec, 1.5f, WHITE);
      }
    }
    EndTextureMode();

    rlImGuiImageRenderTexture(&tex);
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
    } // TextureID selection

    ImGui::End();
  }

  void log() {
    ImGui::Begin("Log");
    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Test");
    ImGui::End();
  }
};
