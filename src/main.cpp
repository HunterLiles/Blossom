#include "Controller.hpp"
#include <Engine.hpp> // IWYU pragma: keep
#include <GLFW/glfw3.h>
#include <Gui.hpp>
// #include <Tilemap.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(void) {
  Window main;
  GLFWwindow *window = main.init();
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Shaders shader;
  shader.LoadShaders("../shaders/vertex.vert", "../shaders/fragment.frag");

  std::vector<float> vertices = {
      0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f,  1.0f, 0.5f, -0.5f, 0.0f,
      0.0f, 1.0f, 0.0f,  1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  1.0f,
      0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f,  1.0f,  0.0f, 0.0f, 1.0f};
  std::vector<unsigned int> indices = {0, 1, 3, 1, 2, 3};
  Mesh triangle;
  triangle.loadBuffers(vertices, indices);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("../resources/container.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else
    std::cerr << "Failed to load texture";
  stbi_image_free(data);

  // // NOTE : Tilemap stuff
  // std::vector<tilemap::MapData> level;
  // for (size_t i{}; i < gui.levels.size(); i++) {
  //   auto levelData = map.init(gui.levels[i]);
  //   level.push_back(levelData);
  // }
  //
  // // NOTE : Enviornment Init
  // Texture2D envTex = LoadTexture("../resources/trees.png");
  // std::vector<Rectangle> envRec = {
  //     {}, {352, 576, 32, 32}, {224, 306, 176, 176}};

  // NOTE : Player Init
  controller cont(1.0f, 1.5f, 3.0f);

  camera camera;
  Math::mat4 view = {.mat{}}, proj = {.mat{}};
  Gui gui(window);

  // NOTE : NPC Init

  float lastTime{}, deltaTime{};

  glUseProgram(shader.shaderProgram);
  while (!glfwWindowShouldClose(window)) {
    // NOTE : Pre-rendering things
    float currTime = glfwGetTime();
    deltaTime = currTime - lastTime;
    lastTime = currTime;

    cont.contr_update(window, camera.eye, camera.center, deltaTime);

    view = camera.viewMat();
    proj = camera.projMat();

    GLint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shader.shaderProgram, "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_TRUE,
                       reinterpret_cast<const GLfloat *>(view.mat.data()));
    glUniformMatrix4fv(projLoc, 1, GL_TRUE,
                       reinterpret_cast<const GLfloat *>(proj.mat.data()));
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "ourTexture"), 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(triangle.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    float lastTime = glfwGetTime();
    float frameTime = lastTime - currTime;

    // NOTE : Background textures
    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     DrawTextureRec(envTex, envRec[level[gui.currLevel].background[i][j]],
    //                    (Vector2){j * 32.0f, i * 32.0f}, WHITE);
    //   }
    // }

    // NOTE : Player and NPC

    // // NOTE : Foreground textures
    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     DrawTextureRec(envTex, envRec[level[gui.currLevel].foreground[i][j]],
    //                    (Vector2){j * 32.0f, i * 32.0f}, WHITE);
    //   }
    // }

    // NOTE : Dynamic UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    gui.settings(camera.eye, frameTime);
    // gui.tile_editor(&level[gui.currLevel], envTex, envRec);
    gui.log();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glDeleteVertexArrays(1, &triangle.VAO);
  glDeleteBuffers(1, &triangle.VBO);
  glDeleteBuffers(1, &triangle.EBO);
  glDeleteProgram(shader.shaderProgram);
  glfwTerminate();
  return 0;
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
