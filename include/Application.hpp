#pragma once

#include "Input.hpp"
#include "Renderer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

int runApplication() {
  Engine engine{};
  try {
    initialize(engine);
    while (!glfwWindowShouldClose(engine.window)) {
      glfwPollEvents();
      const double now = glfwGetTime();
      const float deltaTime = static_cast<float>(std::min(now - engine.previousTime, 0.1));
      engine.previousTime = now;
      engine.frameTime = deltaTime;
      processInput(engine, deltaTime);
      if (glfwWindowShouldClose(engine.window)) break;
      pollShaderReload(engine, now);
      resizeViewTargets(engine);
      buildInterface(engine);
      drawFrame(engine);
      ++engine.renderedFrames;
    }
    shutdown(engine);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    shutdown(engine);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
