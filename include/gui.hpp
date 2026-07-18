#include "imgui.h"
#include "math.hpp"
#include "tilemap.hpp"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class gui {
private:
  std::vector<std::string> texID = {"Air {0}", "Grass {1}", "Tree {2}"};
  int currTex = 0;
  std::vector<std::string> layer = {"background", "foreground"};
  int currLayer = 0;

  // RenderTexture2D tex =
  //     LoadRenderTexture(tilemap::TILE * 32, tilemap::TILE * 32);
  // camera cam = {
  //     .offset = {0.0f, 0.0f},
  //     .target = {0.0f, 0.0f},
  //     .rotation = 0.0f,
  //     .zoom = 1.0f,
  // };

public:
  gui(bool DarkMode) {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  ~gui() = default;

  std::vector<std::string> levels = {"one", "two", "three"};
  int currLevel = 0;

  void settings(math::vec2 pos, float deltaTime) {
    ImGui::Begin("Settings");
    ImGui::Text("FPS : %.2f", 1.0f / deltaTime);
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

  void tile_editor(GLFWwindow *window, tilemap::MapData *map, float zoom,
                   math::vec2 target) {
    ImGui::Begin("Level Editor", nullptr,
                 ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

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
    ImGui::SameLine(0.0f, 10.0f);
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
    ImGui::Dummy(ImVec2(0, 50));
    // rlImGuiImageRenderTexture(&tex);

    zoom = expf(
        logf(zoom) +
        (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) * 0.1f);
    if (zoom > 10.0f)
      zoom = 10.0f;
    else if (zoom < 0.01f)
      zoom = 0.01;

    // if (ImGui::IsItemHovered() &&
    //     glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    //   target =
    //       GetScreenToWorld2D(Vector2Subtract(cam.offset, GetMouseDelta()),
    //       cam);

    // float scaleX = (((float)texture.width / ImGui::GetItemRectSize().x)) /
    // zoom; float scaleY = ((float)texture.height / ImGui::GetItemRectSize().y)
    // / zoom; math::vec2 mousePos = {(ImGui::GetMousePos().x -
    //                         (ImGui::GetItemRectMin().x - (target.x /
    //                         scaleX))) *
    //                            scaleX,
    //                        (ImGui::GetMousePos().y -
    //                         (ImGui::GetItemRectMin().y - (target.y /
    //                         scaleY))) *
    //                            scaleY};
    //
    // target.x = Clamp(target.x, 0.0f, texture.width);
    // target.y = Clamp(target.y, 0.0f, texture.height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};
    //     DrawTextureRec(envTex, envRec[map->background[i][j]],
    //                    (Vector2){j * 32.0f, i * 32.0f}, WHITE);
    //
    //     if (CheckCollisionPointRec(mousePos, gridRec) &&
    //     ImGui::IsItemHovered())
    //       DrawRectangleLinesEx(gridRec, 1.5f, WHITE);
    //
    //     if (ImGui::IsItemHovered() && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    //       int hoverX = floor((int)mousePos.x / 32.0f);
    //       int hoverY = floor((int)mousePos.y / 32.0f);
    //       if (layer[currLayer] == "background")
    //         map->background[hoverY][hoverX] = currTex;
    //     }
    //   }
    // }

    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     Rectangle gridRec = {(j * 32.0f), (i * 32.0f), 32.0f, 32.0f};
    //     DrawTextureRec(envTex, envRec[map->foreground[i][j]],
    //                    (math::vec2){j * 32.0f, i * 32.0f}, WHITE);
    //
    //     if (CheckCollisionPointRec(mousePos, gridRec) &&
    //     ImGui::IsItemHovered())
    //       DrawRectangleLinesEx(gridRec, 1.5f, WHITE);
    //
    //     if (ImGui::IsItemHovered() && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    //       int hoverX = floor((int)mousePos.x / 32.0f);
    //       int hoverY = floor((int)mousePos.y / 32.0f);
    //       if (layer[currLayer] == "foreground")
    //         map->foreground[hoverY][hoverX] = currTex;
    //     }
    //   }
    // }

    ImGui::End();
  }

  void log() {
    ImGui::Begin("Log");
    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Test");
    ImGui::End();
  }
};
