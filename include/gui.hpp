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
  std::vector<std::string> layer = {"background", "foreground"};
  int currLayer = 0;

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

    ImGui::SetNextItemWidth(100);
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
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::BeginCombo("Layer", layer[currLayer].c_str())) {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                          ImVec2(0.5f, 0.5f));

      for (size_t i{}; i < layer.size(); i++) {
        bool isSelected = (currLayer == i);
        if (ImGui::Selectable(layer[i].c_str(), isSelected))
          currLayer = i;
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::PopStyleVar();
      ImGui::EndCombo();
    }

    Vector2 mouse = {GetMousePosition().x - ImGui::GetCursorScreenPos().x,
                     GetMousePosition().y - ImGui::GetCursorScreenPos().y};

    // TODO : Make it only interactable through the visible part of the texture.

    BeginTextureMode(tex);
    ClearBackground(BLACK);
    for (size_t i{}; i < tilemap::TILE; i++) {
      for (size_t j{}; j < tilemap::TILE; j++) {
        Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};
        DrawTextureRec(envTex, envRec[map->background[i][j]],
                       (Vector2){j * 32.0f, i * 32.0f}, WHITE);

        if (CheckCollisionPointRec(mouse, gridRec))
          DrawRectangleLinesEx(gridRec, 1.5f, WHITE);

        if (CheckCollisionPointRec(mouse, gridRec) &&
            IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          if (layer[currLayer] == "background")
            map->background[i][j] = currTex;
        }
      }
    }

    for (size_t i{}; i < tilemap::TILE; i++) {
      for (size_t j{}; j < tilemap::TILE; j++) {
        Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};
        DrawTextureRec(envTex, envRec[map->foreground[i][j]],
                       (Vector2){j * 32.0f, i * 32.0f}, WHITE);

        if (CheckCollisionPointRec(mouse, gridRec))
          DrawRectangleLinesEx(gridRec, 1.5f, WHITE);
        if (CheckCollisionPointRec(mouse, gridRec) &&
            IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          if (layer[currLayer] == "foreground")
            map->foreground[i][j] = currTex;
        }
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
