#pragma once

#include <Core.hpp>
#include <iostream>

class Window {
private:
  void static framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height) {
    glViewport(0, 0, width, height);
  }

public:
  Window() = default;
  ~Window() = default;

  GLFWwindow *window = nullptr;

  void Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Blossom", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create window";
      glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
      std::cout << "Failed to init GLAD";
    }
  }

  void Stop() { glfwTerminate(); }
};
