#version 330 core
layout(location = 0) in vec3 Position_in;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
  gl_Position = lightSpaceMatrix * model * vec4(Position_in, 1.0f);
}
