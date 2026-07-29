#include <Engine.hpp>
using namespace Engine;

int main(void) {
  Init();

  // --------------------
  // TODO :
  // Combine the Mesh and Texture into one call.
  // NOTE :
  //
  // ---------------------
  Mesh triangle;
  triangle.loadBuffers();
  Texture tex;
  tex.LoadTexture("../resources/container.jpg", true, true);

  while (!ShouldWindowClose()) {
    Begin(true);

    // --------------------
    // TODO :
    // Get these gl method calls into one Draw() function for ease of use.
    // NOTE :
    //
    // ---------------------
    glBindTexture(GL_TEXTURE_2D, tex.texture);
    glBindVertexArray(triangle.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    End();
  }

  triangle.Stop();
  Stop();
  return 0;
}
