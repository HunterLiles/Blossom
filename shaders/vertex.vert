#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform vec3 ourColor;
uniform vec2 TexCoord;
uniform mat4 view, proj;

void main() {
  gl_Position = proj * view * vec4(aPos, 1.0);
  ourColor = aColor;
  TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
