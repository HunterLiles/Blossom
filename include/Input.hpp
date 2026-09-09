#pragma once

#include "Engine.hpp"

namespace {
bool keyDown(const Engine& engine, int key) { return glfwGetKey(engine.window, key) == GLFW_PRESS; }
}

void setCursorCapture(Engine& engine, bool captured) {
  engine.cursorCaptured = captured;
  engine.firstMouse = true;
  // GLFW_CURSOR_DISABLED hides the cursor and provides relative camera input.
  glfwSetInputMode(engine.window, GLFW_CURSOR,
                   captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(engine.window, GLFW_RAW_MOUSE_MOTION, captured ? GLFW_TRUE : GLFW_FALSE);
}

void processInput(Engine& engine, float deltaTime) {
  const bool neovimOwnsKeyboard = engine.neovimWindowOpen && engine.neovimKeyboardFocused;
  const bool escapeDown = keyDown(engine, GLFW_KEY_ESCAPE);
  if (!neovimOwnsKeyboard && escapeDown && !engine.escapeWasDown) {
    if (engine.exitConfirmationOpen) {
      engine.exitConfirmationOpen = false;
      if (engine.resumeInputAfterExitConfirmation) setCursorCapture(engine, true);
      engine.resumeInputAfterExitConfirmation = false;
    } else {
      engine.exitConfirmationOpen = true;
      engine.resumeInputAfterExitConfirmation = engine.cursorCaptured;
      if (engine.cursorCaptured) setCursorCapture(engine, false);
    }
  }
  engine.escapeWasDown = escapeDown;
  const bool enterDown = keyDown(engine, GLFW_KEY_ENTER) || keyDown(engine, GLFW_KEY_KP_ENTER);
  if (!neovimOwnsKeyboard && engine.exitConfirmationOpen && enterDown && !engine.enterWasDown)
    glfwSetWindowShouldClose(engine.window, GLFW_TRUE);
  engine.enterWasDown = enterDown;
  if (engine.exitConfirmationOpen && !neovimOwnsKeyboard) return;
  const bool tabDown = keyDown(engine, GLFW_KEY_TAB);
  if (!neovimOwnsKeyboard && tabDown && !engine.tabWasDown) setCursorCapture(engine, !engine.cursorCaptured);
  engine.tabWasDown = tabDown;
  const bool shaderReloadDown = keyDown(engine, GLFW_KEY_F5);
  if (!neovimOwnsKeyboard && shaderReloadDown && !engine.shaderReloadWasDown) engine.shaderReloadRequested = true;
  engine.shaderReloadWasDown = shaderReloadDown;
  if (engine.cursorCaptured && !neovimOwnsKeyboard) {
    Camera& camera = engine.focusedView == ViewKind::Scene ? engine.sceneCamera : engine.camera;
    double mouseX = 0.0, mouseY = 0.0;
    glfwGetCursorPos(engine.window, &mouseX, &mouseY);
    if (!engine.firstMouse) {
      const float yaw = static_cast<float>(engine.mouseX - mouseX) * camera.sensitivity;
      const float pitch = static_cast<float>(engine.mouseY - mouseY) * camera.sensitivity;
      camera.orientation = math::normalize(math::multiply(math::axisAngle({0.0f, 1.0f, 0.0f}, yaw), math::multiply(camera.orientation, math::axisAngle({1.0f, 0.0f, 0.0f}, pitch))));
    }
    engine.mouseX = mouseX;
    engine.mouseY = mouseY;
    engine.firstMouse = false;
    math::Vec3 movement{};
    const math::Vec3 forward = math::rotate(camera.orientation, {0.0f, 0.0f, -1.0f});
    const math::Vec3 right = math::rotate(camera.orientation, {1.0f, 0.0f, 0.0f});
    if (keyDown(engine, GLFW_KEY_W)) movement = math::add(movement, forward);
    if (keyDown(engine, GLFW_KEY_S)) movement = math::subtract(movement, forward);
    if (keyDown(engine, GLFW_KEY_D)) movement = math::add(movement, right);
    if (keyDown(engine, GLFW_KEY_A)) movement = math::subtract(movement, right);
    if (keyDown(engine, GLFW_KEY_SPACE)) movement.y += 1.0f;
    if (keyDown(engine, GLFW_KEY_LEFT_CONTROL) || keyDown(engine, GLFW_KEY_RIGHT_CONTROL)) movement.y -= 1.0f;
    const float speed = keyDown(engine, GLFW_KEY_LEFT_SHIFT) ? camera.speed * 2.5f : camera.speed;
    camera.position = math::add(camera.position, math::scale(math::normalize(movement), speed * deltaTime));
  }
  engine.cube.angle += deltaTime * 0.65f;
}
