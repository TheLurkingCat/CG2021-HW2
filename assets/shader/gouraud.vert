#version 330 core
layout(location = 0) in vec3 Position_in;
layout(location = 1) in vec3 Normal_in;
layout(location = 2) in vec2 TextureCoordinate_in;
// Gouraud shading will calculate color in vs
out vec4 vertexColor;
// Model matrix
uniform mat4 model;
// Projection * View matrix
uniform mat4 viewProjection;
uniform mat4 lightSpaceMatrix;
// inverse(transpose(model)), precalculate using CPU for efficiency
uniform mat4 normalMatrix;
// Color of object
uniform sampler2D diffuseTexture;
uniform sampler2DShadow shadowMap;
// Position of the light
uniform vec4 lightVector;
// Position of the camera
uniform vec4 viewPosition;

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
  vec4 vertexPosition = model * vec4(Position_in, 1.0);
  vec3 vertexNormal = normalize(mat3(normalMatrix) * Normal_in);
  vec4 lightSpacePosition = lightSpaceMatrix * vertexPosition;
  gl_Position = viewProjection * vertexPosition;

  // Calculate some directions
  // Directional or Positioal light
  vec3 lightDirection;
  float attenuation;
  if (lightVector.w == 0.0) {
    lightDirection = normalize(lightVector.xyz);
    attenuation = 0.65;
  } else {
    lightDirection = normalize((lightVector - vertexPosition).xyz);
    float distance = length(lightVector.xyz - vertexPosition.xyz);
    attenuation = 1.0 / (1.0 + 0.027 * distance + 0.0028 * (distance * distance));
  }
  vec3 viewDirection = normalize((viewPosition - vertexPosition).xyz);
  vec3 reflectDirection = reflect(-lightDirection, vertexNormal);

  // Ambient intensity
  float ambient = 0.1;
  // Diffuse intensity
  float normalDotLight = dot(vertexNormal, lightDirection);
  float diffuse = 0.75 * max(normalDotLight, 0.0);
  // Specular intensity
  float specular = 0.75 * pow(max(dot(viewDirection, reflectDirection), 0.0), 8.0);

  vec3 perspectiveDivision = lightSpacePosition.xyz / lightSpacePosition.w;
  float shadow = perspectiveDivision.z > 1.0 ? 1.0 : calculateShadow(perspectiveDivision, normalDotLight);
  vec3 lighting = attenuation * (ambient + shadow * (diffuse + specular)) * texture(diffuseTexture, TextureCoordinate_in).rgb;
  vertexColor = vec4(lighting, 1.0);
}
