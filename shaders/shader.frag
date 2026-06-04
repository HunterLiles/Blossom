#version 330

in vec2 fragCoord;
out vec4 fragColor;

// Uniforms go here.
uniform sampler2D texture0;
uniform float time;
uniform vec2 resolution;

const int scanlineCount = 200;

void main() {
  vec2 uv = fragCoord / resolution;

  fragColor = vec4(abs(sin(time)));
}
