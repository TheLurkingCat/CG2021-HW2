#version 330 core
layout(location = 0) out vec4 FragColor;

in VS_OUT {
  vec3 Position;
  vec3 Normal;
  vec2 TextureCoordinate;
  vec4 LightSpacePosition;
} fs_in;

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
  vec3 normal = normalize(fs_in.Normal);
  // Calculate some directions
  // Directional or Positioal light
  vec3 lightDirection;
  float attenuation;
  if (lightVector.w == 0.0) {
    lightDirection = normalize(lightVector.xyz);
    attenuation = 1.0;
  } else {
    lightDirection = normalize(lightVector.xyz - fs_in.Position);
    float distance = length(lightVector.xyz - fs_in.Position);
    attenuation = 1.0 / (1.0 + 0.027 * distance +  0.0028 * (distance * distance));
  }
  vec3 viewDirection = normalize(viewPosition.xyz - fs_in.Position);
  vec3 reflectDirection = reflect(-lightDirection, normal);
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);
  // Ambient intensity
  float ambient = 0.1;
  // Diffuse intensity
  float normalDotLight = dot(normal, lightDirection);
  float diffuse = 0.75 * max(normalDotLight, 0.0);
  // Specular intensity (Phong)
  // float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), 8.0);
  // Specular intensity (Blinn-Phong)
  float specular = 0.75 * pow(max(dot(normal, halfwayDirection), 0.0), 8.0);
  // Shadow
  vec3 perspectiveDivision = fs_in.LightSpacePosition.xyz / fs_in.LightSpacePosition.w;
  float shadow = perspectiveDivision.z > 1.0 ? 1.0 : calculateShadow(perspectiveDivision, normalDotLight);

  vec3 lighting = attenuation * (ambient + shadow * (diffuse + specular)) * texture(diffuseTexture, fs_in.TextureCoordinate).rgb;
  FragColor = vec4(lighting, 1.0);
}
