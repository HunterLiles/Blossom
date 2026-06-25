#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

#include <string>
#include <vector>

class gui {

private:
public:
  gui() {
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  ~gui() { rlImGuiShutdown(); };

  void guiSettings(Vector2 pos, std::vector<std::string> levels,
                   int currLevel) {
    rlImGuiBegin();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::SetNextWindowPos({100, 50}, ImGuiCond_Once);
    ImGui::Begin("Settings");
    ImGui::Text("FPS : %.2d", GetFPS());
    ImGui::Text("X : %.2f | Y : %.2f", pos.x, pos.y);
    ImGui::End();

    ImGui::SetNextWindowPos({100, 150}, ImGuiCond_Once);
    ImGui::Begin("Level");
    if (ImGui::BeginCombo("Level", levels[currLevel].c_str())) {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                          ImVec2(0.5f, 0.5f));

      for (int n = 0; n < levels.size(); n++) {
        bool is_selected = (currLevel == n);
        if (ImGui::Selectable(levels[n].c_str(), is_selected)) {
          currLevel = n;
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::PopStyleVar();
      ImGui::EndCombo();
    }

    ImGui::End();

    rlImGuiEnd();
  }
};
