#pragma once

#include <Core.hpp>
#include <iostream>

class Window {
private:
public:
  Window() = default;
  ~Window() = default;

  GLFWwindow *window = nullptr;
  GLFWwindow *init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Blossom", NULL, NULL);
    if (window == NULL) {
      std::cerr << "Failed to create window";
      glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
      std::cerr << "Failed to init GLAD";
    }
    return window;
  }
};
