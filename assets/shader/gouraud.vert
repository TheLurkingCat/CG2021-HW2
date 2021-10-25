#version 330 core
layout(location = 0) in vec3 Position_in;
layout(location = 1) in vec3 Normal_in;
layout(location = 2) in vec2 TextureCoordinate_in;
// Gouraud shading will calculate color in vs
out vec3 rawPosition;
out float colorFactor;
out vec2 TextureCoordinate;

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
// precomputed shadow
uniform sampler2DShadow shadowMap;

float calculateShadow(vec3 projectionCoordinate, float normalDotLight) {
    // Domain transformation to [0, 1]
    projectionCoordinate = projectionCoordinate * 0.5 + 0.5;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec3 textureCoord = vec3(projectionCoordinate.xy + vec2(x, y) * texelSize, projectionCoordinate.z);
            shadow += texture(shadowMap, textureCoord);
        }
    }
    return 0.25 + shadow / 12.0;
}

void main() {
  // Transform vertices
  vec4 vertexPosition = modelMatrix * vec4(Position_in, 1.0);
  vec4 vertexNormal = normalize(normalMatrix * vec4(Normal_in, 0.0));
  vec4 lightSpacePosition = lightSpaceMatrix * vertexPosition;
  gl_Position = viewProjectionMatrix * vertexPosition;

  // Calculate some directions
  // Directional or Positioal light
  vec4 lightDirection;
  vec4 viewDirection = normalize(viewPosition - vertexPosition);
  float attenuation;
  if (coefficients.z != 0){
    // Spotlight use lightVector as direction
    lightDirection = -normalize(lightVector);
    float theta = dot(lightDirection, viewDirection);
    float epsilon = coefficients.x - coefficients.y;
    float distance = length(viewPosition - vertexPosition);
    float intensity = clamp((theta - coefficients.y) / epsilon, 0.0, 1.0);
    attenuation = intensity / (1.0 + 0.014 * distance + 0.0007 * (distance * distance));
  } else if (coefficients.w != 0.0) {
    // Directionlight use lightVector as direction
    lightDirection = normalize(lightVector);
    attenuation = 0.65;
  } else {
    // Pointlight use lightVector as position
    lightDirection = normalize(lightVector - vertexPosition);
    float distance = length(lightVector - vertexPosition);
    attenuation = 1.0 / (1.0 + 0.027 * distance + 0.0028 * (distance * distance));
  }
  vec4 reflectDirection = reflect(-lightDirection, vertexNormal);

  // Ambient intensity
  float ambient = 0.1;
  // Diffuse intensity
  float normalDotLight = dot(vertexNormal, lightDirection);
  float diffuse = 0.75 * max(normalDotLight, 0.0);
  // Specular intensity
  float specular = 0.75 * pow(max(dot(viewDirection, reflectDirection), 0.0), 8.0);

  vec3 perspectiveDivision = lightSpacePosition.xyz / lightSpacePosition.w;
  float shadow = perspectiveDivision.z > 1.0 ? 1.0 : calculateShadow(perspectiveDivision, normalDotLight);
  colorFactor = (ambient + attenuation * shadow * (diffuse + specular));
  TextureCoordinate = TextureCoordinate_in;
  rawPosition = mat3(modelMatrix) * Position_in;
}
