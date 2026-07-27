#include <Engine.hpp> // IWYU pragma: keep
#include <Tilemap.hpp>

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
  triangle.loadTexture("../resources/player-anim/idle.png");

  // NOTE : Tilemap stuff

  // NOTE : Enviornment Init

  // NOTE : Player Init
  controller cont(1.0f, 1.5f, 3.0f);

  camera camera;
  Math::mat4 view = {.mat{}}, proj = {.mat{}};

  // NOTE : NPC Init

  Gui gui;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  float lastTime{}, deltaTime{};

  glUseProgram(shader.shaderProgram);
  glUniform1i(glGetUniformLocation(shader.shaderProgram, "ourTexture"), 0);

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, triangle.texture);
    glUseProgram(shader.shaderProgram);
    glBindVertexArray(triangle.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // NOTE : Background textures

    // NOTE : Player and NPC

    // NOTE : Foreground textures

    // NOTE : Dynamic UI

    float lastTime = glfwGetTime();
    float frameTime = lastTime - currTime;

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
