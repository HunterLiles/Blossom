#pragma once

#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

class Shaders {
private:
  std::string ReadFile(std::string_view filepath) {
    std::ifstream file(filepath.data());
    std::stringstream s;

    if (!file.is_open()) {
      std::cerr << "Error opening shader file";
    }

    s << file.rdbuf();
    file.close();
    return s.str();
  }

public:
  Shaders() = default;
  ~Shaders() = default;
  GLint shaderProgram;
  Shaders LoadShaders(std::string_view vertPath, std::string_view fragPath) {
    std::string vertShader = ReadFile(vertPath);
    const char *vert = vertShader.c_str();
    GLint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vert, NULL);
    glCompileShader(vertex);

    std::string fragShader = ReadFile(fragPath);
    const char *frag = fragShader.c_str();
    GLint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &frag, NULL);
    glCompileShader(fragment);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return {shaderProgram};
  }
};
