#include "gui.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void guiSettings(Vector2 pos) {
  int current = 0;
  GuiPanel((Rectangle){10, 10, 300, 200}, "Settings");
  GuiLabel((Rectangle){15, 50, 1000, 1}, TextFormat("FPS: %i", GetFPS()));
  GuiLabel((Rectangle){15, 70, 150, 1},
           TextFormat("x: %.2f, y: %.2f", pos.x, pos.y));
  GuiDropdownBox((Rectangle){15, 90, 175, 10}, "Levels", &current, false);
}
