#pragma once

#include <Core.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

#include <Math.hpp>

class Gui {
private:
public:
  Gui() = default;
  ~Gui() = default;

  void Stop() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void Init(GLFWwindow *window, bool isDocking, std::string version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    if (isDocking)
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(version.c_str());
  }

  void Start(bool isDocking) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (isDocking)
      ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                   ImGuiDockNodeFlags_PassthruCentralNode);
  }
  void End() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  void Settings(Math::vec3 pos, float frameTime) {
    ImGui::Begin("Settings");

    ImGui::Text("FPS : %.0f", 1 / frameTime);
    ImGui::Text("Frame Time : %.10f", frameTime);
    ImGui::Text("X : %.2f | Y : %.2f | Z : %.2f", pos.x, pos.y, pos.z);

    ImGui::End();
  }

  void Log() {
    ImGui::Begin("Log");
    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Test");
    ImGui::End();
  }
};
