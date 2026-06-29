#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "tilemap.hpp"

#include <string>
#include <vector>

using namespace ImGui;

class gui {
private:
public:
  gui(bool DarkMode) {
    rlImGuiSetup(DarkMode);
    GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  ~gui() { rlImGuiShutdown(); };

  void settings(Vector2 pos, std::vector<std::string> levels, int *currLevel) {
    DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode);

    Begin("Settings");
    Text("FPS : %.2d", GetFPS());
    Text("X : %.2f | Y : %.2f", pos.x, pos.y);

    if (BeginCombo("Level", levels[*currLevel].c_str())) {
      PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

      for (int n = 0; n < levels.size(); n++) {
        bool is_selected = (*currLevel == n);
        if (Selectable(levels[n].c_str(), is_selected))
          *currLevel = n;

        if (is_selected)
          SetItemDefaultFocus();
      }
      PopStyleVar();
      EndCombo();
    }

    End();
  }

  tilemap::MapData tile_editor() {
    // NOTE : How do I draw a grid in ImGui?

    // NOTE : How do I make that grid interactable?

    // NOTE : How do I save the information back to the tilemap (Maybe a temp
    // array that is a copy of the currently opened one?) and save that to the
    // binary file?

    tilemap::MapData tempMap;
    // TODO : This is the temp array, how do I apply the current textureid to
    // the location of the click in the array?
    return tempMap;
  }
};
