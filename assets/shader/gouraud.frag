#version 330 core
layout(location = 0) out vec4 FragColor;
// Data from vertex shader
in vec3 rawPosition;
in float colorFactor;
in vec2 TextureCoordinate;

// Texture of object
uniform sampler2D diffuseTexture;
uniform samplerCube diffuseCubeTexture;
uniform int isCube;

void main() {
  vec4 diffuseTextureColor = texture(diffuseTexture, TextureCoordinate);
  vec4 diffuseCubeTextureColor = texture(diffuseCubeTexture, rawPosition);
  vec3 color = isCube == 1 ? diffuseCubeTextureColor.rgb : diffuseTextureColor.rgb;
  vec3 lighting = colorFactor * color;
  FragColor = vec4(lighting, 1.0);
}
