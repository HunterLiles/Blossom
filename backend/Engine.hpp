#pragma once

// IWYU pragma: begin_exports
#include <Window.hpp>

#include <Animation.hpp>
#include <Camera.hpp>
#include <Gui.hpp>
#include <Math.hpp>
#include <Mesh.hpp>
#include <Shaders.hpp>
#include <Texture.hpp>
#include <Tilemap.hpp>
// IWYU pragma: end_exports

namespace Engine {
inline Window windowSys;
inline GLFWwindow *window = nullptr;
inline Shaders *shader = nullptr;
inline Camera cam;
inline Gui gui;

inline float lastTime{}, deltaTime{}, currTime{};
inline bool isUi;

inline void Init() {
  windowSys.Init();
  window = windowSys.window;
  shader = new Shaders("../backend/shaders/vertex.vert",
                       "../backend/shaders/fragment.frag");
  gui.Init(window, true, "#version 330");
}

// -----------------------
// TODO :
// In Begin start rendering the OpenGL output to a texture, in End finish that
// and pass it to ImGui so that the rendered image can be shown in an ImGui
// window.
// NOTE :
//
// -----------------------
inline void Begin(bool isGui) {
  isUi = isGui;
  currTime = glfwGetTime();
  deltaTime = currTime - lastTime;
  lastTime = currTime;

  cam.Controller(window, deltaTime);
  cam.Update(shader->ID);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shader->use();
}

inline void End() {
  float lastFrame = glfwGetTime();
  float frameTime = lastFrame - currTime;

  if (isUi) {
    gui.Start(true);
    gui.Settings(cam.eye, frameTime);
    gui.Log();
    gui.End();
  }

  glfwSwapBuffers(window);
  glfwPollEvents();
}

inline void Stop() {
  delete shader;
  gui.Stop();
  windowSys.Stop();
}

inline bool ShouldWindowClose() {
  if (glfwWindowShouldClose(window))
    return true;
  return false;
}

inline bool GetKeyDown(int key) {
  return glfwGetKey(window, key) == GLFW_PRESS;
}
} // namespace Engine
