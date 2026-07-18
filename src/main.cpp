#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include "camera.hpp"
// #include "animation.hpp"
#include "controller.hpp"
#include "gui.hpp"
#include "math.hpp"
// #include "imgui.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
std::string ReadFile(std::string file);

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(1280, 720, "Blossom", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::cerr << "Failed to init GLAD";
    return -1;
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  std::string vert = ReadFile("../shaders/vertex.vert");
  const char *vertSrc = vert.c_str();
  unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, &vertSrc, NULL);
  glCompileShader(vertShader);

  std::string frag = ReadFile("../shaders/fragment.frag");
  const char *fragSrc = frag.c_str();
  unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragSrc, NULL);
  glCompileShader(fragShader);

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertShader);
  glAttachShader(shaderProgram, fragShader);
  glLinkProgram(shaderProgram);
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // gui gui(true);
  // tilemap map;
  camera cam;
  cam.pos = {0.0f, 0.0f};
  cam.target = {0.0f, 0.0f};
  cam.zoom = 1.0f;

  controller cont;
  cont.speed = 10.0f;
  cont.sprint = 1.5f;

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  math::mat4 view = {.mat = {0.0f}};
  math::mat4 proj = {.mat = {0.0f}};

  // NOTE : Tilemap stuff
  // std::vector<tilemap::MapData> level;
  // for (size_t i{}; i < gui.levels.size(); i++) {
  //   auto levelData = map.init(gui.levels[i]);
  //   level.push_back(levelData);
  // }

  // NOTE : Enviornment Init
  // Texture2D envTex = LoadTexture("../resources/trees.png");
  // std::vector<Rectangle> envRec = {
  //     {}, {352, 576, 32, 32}, {224, 306, 176, 176}};

  // NOTE : Player Init

  // NOTE : NPC Init

  while (!glfwWindowShouldClose(window)) {
    // NOTE : Pre-rendering things
    float currFrame = glfwGetTime();
    deltaTime = currFrame - lastFrame;
    lastFrame = currFrame;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "proj");

    cam.looAt(view);
    cam.projection(window, proj);
    cont.contr_update(window, cam.pos, cam.zoom, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view.mat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, &proj.mat[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // NOTE : Background textures
    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     DrawTextureRec(envTex, envRec[level[gui.currLevel].background[i][j]],
    //                    (math::vec2){j * 32.0f, i * 32.0f}, WHITE);
    //   }
    // }

    // NOTE : Player and NPC
    // DrawTextureRec(playerAnim.tex[player.state], playerAnim.rec, player.pos,
    //                WHITE);

    // NOTE : Foreground textures
    // for (size_t i{}; i < tilemap::TILE; i++) {
    //   for (size_t j{}; j < tilemap::TILE; j++) {
    //     DrawTextureRec(envTex, envRec[level[gui.currLevel].foreground[i][j]],
    //                    (math::vec2){j * 32.0f, i * 32.0f}, WHITE);
    //   }
    // }

    // NOTE : Dynamic UI

    // ImGui::DockSpaceOverViewport(0, NULL,
    //                              ImGuiDockNodeFlags_PassthruCentralNode);

    // ImGui::Begin("Game Window");
    // rlImGuiImageRenderTextureFit(&gameTex, true);
    // ImGui::End();

    // gui.settings(player.pos, deltaTime);
    // gui.tile_editor(&level[gui.currLevel], envTex, envRec);
    // gui.log();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

std::string ReadFile(std::string filename) {
  std::ifstream file(filename.data());
  if (!file.is_open())
    std::cerr << "Error opening file" << std::endl;
  std::stringstream s;
  s << file.rdbuf();
  return s.str();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
