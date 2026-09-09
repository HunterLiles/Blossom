#pragma once

#include "Engine.hpp"

#include <imgui.h>
#include <imgui_internal.h>

inline void beginWorkspaceDockspace(Engine& engine) {
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  const ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  ImGui::Begin("Blossom Workspace", nullptr, hostFlags);
  ImGui::PopStyleVar(2);
  const ImGuiID dockspace = ImGui::GetID("BlossomDockspace");
  if (!engine.dockLayoutInitialized || ImGui::DockBuilderGetNode(dockspace) == nullptr) {
    ImGui::DockBuilderRemoveNode(dockspace);
    ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace, viewport->WorkSize);
    ImGuiID left = 0, center = 0, projects = 0, utility = 0, controls = 0, inspector = 0, views = 0, game = 0, scene = 0;
    ImGui::DockBuilderSplitNode(dockspace, ImGuiDir_Left, 0.28f, &left, &center);
    ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.43f, &projects, &left);
    ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.38f, &utility, &controls);
    ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.24f, &inspector, &views);
    ImGui::DockBuilderSplitNode(views, ImGuiDir_Down, 0.50f, &game, &scene);
    ImGui::DockBuilderDockWindow("Controls", controls);
    ImGui::DockBuilderDockWindow("Camera", utility);
    ImGui::DockBuilderDockWindow("Engine", utility);
    ImGui::DockBuilderDockWindow("Projects", projects);
    ImGui::DockBuilderDockWindow("Scene", scene);
    ImGui::DockBuilderDockWindow("Game", game);
    ImGui::DockBuilderDockWindow("Inspector: Cube", inspector);
    ImGui::DockBuilderFinish(dockspace);
    engine.dockLayoutInitialized = true;
  }
  ImGui::DockSpace(dockspace, {0.0f, 0.0f}, ImGuiDockNodeFlags_None);
  ImGui::End();
}
