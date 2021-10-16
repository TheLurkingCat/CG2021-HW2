#version 330 core
layout(location = 0) in vec3 Position_in;
layout(location = 1) in vec3 Normal_in;
layout(location = 2) in vec2 TextureCoordinate_in;

out VS_OUT {
  vec3 Position;
  vec3 Normal;
  vec2 TextureCoordinate;
  vec4 LightSpacePosition;
} vs_out;

uniform mat4 model;
uniform mat4 viewProjection;
uniform mat4 lightSpaceMatrix;
uniform mat4 normalMatrix;

void main() {
  vs_out.Position = vec3(model * vec4(Position_in, 1.0));
  vs_out.Normal = mat3(normalMatrix) * Normal_in;
  vs_out.TextureCoordinate = TextureCoordinate_in;
  vs_out.LightSpacePosition = lightSpaceMatrix * vec4(vs_out.Position, 1.0);
  gl_Position = viewProjection * model * vec4(Position_in, 1.0);
}
