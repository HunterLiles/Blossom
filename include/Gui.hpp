#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Math.hpp>
#include <string>
#include <vector>

class Gui {
private:
  std::vector<std::string> texID = {"Air {0}", "Grass {1}", "Tree {2}"};
  int currTex = 0;
  std::string layer[2] = {"background", "foreground"};
  int currLayer = 0;

public:
  Gui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
  };
  ~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  };

  std::vector<std::string> levels = {"one", "two", "three"};
  int currLevel = 0;

  void settings(Math::vec3 pos, float frameTime, bool *is3D) {
    ImGui::Begin("Settings");

    ImGui::Text("FPS : %.2f", 1.0f / frameTime);
    ImGui::Text("FrameTime : %.5f", frameTime);
    ImGui::Text("X : %.2f | Y : %.2f | Z : %.2f", pos.x, pos.y, pos.z);
    ImGui::Checkbox("3D", is3D);

    ImGui::End();
  }

  void log() {
    ImGui::Begin("Log");

    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Test");

    ImGui::End();
  }
};
