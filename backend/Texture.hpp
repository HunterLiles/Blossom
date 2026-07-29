#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Core.hpp>
#include <iostream>

class Texture {
private:
public:
  int width, height;
  int nrChannel;

  unsigned int texture;

  // ------------------------
  // TODO :
  // How can I allow this to dynamically switch for the type of file it is
  // (i.e. png, jpg, etc)?
  // NOTE :
  //
  // ------------------------
  void LoadTexture(std::string filename, bool isRepeat, bool isLinear) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (isRepeat) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    if (isLinear) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    unsigned char *data =
        stbi_load(filename.c_str(), &width, &height, &nrChannel, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else
      std::cout << "Failed to load texture" << "\n";

    stbi_image_free(data);
  }
};
