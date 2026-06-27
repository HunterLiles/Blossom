#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

#include <string>
#include <vector>

class gui {

private:
public:
  gui() { rlImGuiSetup(true); };
  ~gui() { rlImGuiShutdown(); };

  void guiSettings(Vector2 pos, std::vector<std::string> levels,
                   int *currLevel) {
    ImGui::Begin("Settings");
    ImGui::Text("FPS : %.2d", GetFPS());
    ImGui::Text("X : %.2f | Y : %.2f", pos.x, pos.y);

    if (ImGui::BeginCombo("Level", levels[*currLevel].c_str())) {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                          ImVec2(0.5f, 0.5f));

      for (int n = 0; n < levels.size(); n++) {
        bool is_selected = (*currLevel == n);
        if (ImGui::Selectable(levels[n].c_str(), is_selected)) {
          *currLevel = n;
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::PopStyleVar();
      ImGui::EndCombo();
    }

    ImGui::End();
  }
};
