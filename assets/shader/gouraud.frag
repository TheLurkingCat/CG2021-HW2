#version 330 core
layout(location = 0) out vec4 FragColor;
// Data from vertex shader
in vec4 vertexColor;

void main() {
  FragColor = vertexColor;
}
