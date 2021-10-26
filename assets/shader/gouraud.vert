#version 330 core
// x, y, z
layout(location = 0) in vec3 Position_in;
// x, y, z
layout(location = 1) in vec3 Normal_in;
// u, v
layout(location = 2) in vec2 TextureCoordinate_in;
// Hint: Gouraud shading calculates per vertex color, interpolate in fs
out vec3 rawPosition;
out float colorFactor;
out vec2 TextureCoordinate;

// Uniform blocks
// https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)

layout (std140) uniform model {
  // Model matrix
  mat4 modelMatrix;
  // mat4(inverse(transpose(mat3(modelMatrix)))), precalculate using CPU for efficiency
  mat4 normalMatrix;
};

layout (std140) uniform camera {
  // Camera's projection * view matrix
  mat4 viewProjectionMatrix;
  // Position of the camera
  vec4 viewPosition;
};

layout (std140) uniform light {
  // Light's projection * view matrix
  // Hint: If you want to implement shadow, you may use this.
  mat4 lightSpaceMatrix;
  // Position or direction of the light
  vec4 lightVector;
  // inner cutoff, outer cutoff, isSpotlight, isDirectionalLight
  vec4 coefficients;
};

// precomputed shadow
// Hint: You may want to uncomment this to use shader map texture.
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
  // TODO: vertex shader / fragment shader
  // Hint:
  //       1. how to write a vertex shader:
  //          a. The output is gl_Position and anything you want to pass to the fragment shader. (Apply matrix multiplication yourself)
  //       2. how to write a fragment shader:
  //          a. The output is FragColor (any var is OK)
  //       3. colors
  //          a. For point light & directional light, lighting = ambient + attenuation * shadow * (diffuse + specular)
  //          b. If you want to implement multiple light sources, you may want to use lighting = shadow * attenuation * (ambient + (diffuse + specular))
  //       4. attenuation
  //          a. spotlight & pointlight: see spec
  //          b. directional light = no
  //          c. Use formula from slides 'shading.ppt' page 20
  //       5. spotlight cutoff: inner and outer from coefficients.x and coefficients.y
  //       6. diffuse = kd * max(normal vector dot light direction, 0.0)
  //       7. specular = ks * pow(max(normal vector dot halfway direction), 0.0), 8.0);
  //       8. notice the difference of light direction & distance between directional light & point light
  //       9. we've set ambient & color for you

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
