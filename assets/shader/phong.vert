#version 330 core
layout(location = 0) in vec3 Position_in;
layout(location = 1) in vec3 Normal_in;
layout(location = 2) in vec2 TextureCoordinate_in;

out VS_OUT {
  vec3 Position;
  vec3 Normal;
  vec2 TextureCoordinate;
  vec4 LightSpacePosition;
  flat vec4 lightVector;
  flat vec4 viewPosition;
  flat vec4 lightCoefficients;
  vec3 rawPosition;
} vs_out;

layout (std140) uniform model {
  // Model matrix
  mat4 modelMatrix;
  // inverse(transpose(model)), precalculate using CPU for efficiency
  mat4 normalMatrix;
};

layout (std140) uniform camera {
  // Projection * View matrix
  mat4 viewProjectionMatrix;
  // Position of the camera
  vec4 viewPosition;
};

layout (std140) uniform light {
  // Projection * View matrix
  mat4 lightSpaceMatrix;
  // Position or direction of the light
  vec4 lightVector;
  // inner cutoff, outer cutoff, isSpotlight, isPointLight
  vec4 coefficients;
};

void main() {
  vs_out.Position = vec3(modelMatrix * vec4(Position_in, 1.0));
  vs_out.Normal = mat3(normalMatrix) * Normal_in;
  vs_out.TextureCoordinate = TextureCoordinate_in;
  vs_out.LightSpacePosition = lightSpaceMatrix * vec4(vs_out.Position, 1.0);
  vs_out.lightVector = lightVector;
  vs_out.viewPosition = viewPosition;
  vs_out.lightCoefficients = coefficients;
  vs_out.rawPosition = mat3(modelMatrix) * Position_in;
  gl_Position = viewProjectionMatrix * modelMatrix * vec4(Position_in, 1.0);
}
