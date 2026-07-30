#include <Engine.hpp>

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

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int VBO{}, VAO{}, EBO{};
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  {
    Gui gui(window);
    camera cam;

    float deltaTime{}, lastTime{};
    bool is3D{false};

    while (!glfwWindowShouldClose(window)) {
      // NOTE : Pre-rendering things
      float currTime = glfwGetTime();
      deltaTime = currTime - lastTime;
      lastTime = currTime;

      glClearColor(0.4f, 0.4f, 0.6f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(shaderProgram);

      if (is3D) {
        cam.Controller(window, deltaTime); // This needs to change
        cam.lookAt();
        cam.perspective();
      } else {
        cam.Controller(window, deltaTime);
        cam.lookAt();
        cam.orthographic();
      }

      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1,
                         GL_TRUE, &cam.view.mat[0].x);
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1,
                         GL_TRUE, &cam.proj.mat[0].x);

      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      float lastFrame = glfwGetTime();
      float frameTime = lastFrame - currTime;

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      gui.settings(cam.eye, frameTime, &is3D);
      gui.log();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }
  glDeleteProgram(shaderProgram);
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
