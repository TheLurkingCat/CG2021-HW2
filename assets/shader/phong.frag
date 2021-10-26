#version 330 core
layout(location = 0) out vec4 FragColor;

in VS_OUT {
  vec3 Position;
  vec3 Normal;
  vec2 TextureCoordinate;
  vec4 LightSpacePosition;
  flat vec4 lightVector;
  flat vec4 viewPosition;
  flat vec4 lightCoefficients;
  vec3 rawPosition;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube diffuseCubeTexture;
uniform sampler2DShadow shadowMap;
uniform int isCube;

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

  vec3 normal = normalize(fs_in.Normal);
  // Calculate some directions
  // Directional or Positioal light
  vec3 lightDirection;
  vec3 viewDirection = normalize(fs_in.viewPosition.xyz - fs_in.Position);
  float attenuation;
  if (fs_in.lightCoefficients.z != 0) {
    // Spotlight use lightVector as direction
    lightDirection = -normalize(fs_in.lightVector.xyz);
    float theta = dot(lightDirection, viewDirection);
    float epsilon = fs_in.lightCoefficients.x - fs_in.lightCoefficients.y;
    float distance = length(fs_in.viewPosition.xyz - fs_in.Position);
    float intensity = clamp((theta - fs_in.lightCoefficients.y) / epsilon, 0.0, 1.0);
    attenuation = intensity / (1.0 + 0.014 * distance + 0.0007 * (distance * distance));
  } else if (fs_in.lightCoefficients.w != 0) {
    // Directionlight use lightVector as direction
    lightDirection = normalize(fs_in.lightVector.xyz);
    attenuation = 0.65;
  } else {
    // Pointlight use lightVector as position
    lightDirection = normalize(fs_in.lightVector.xyz - fs_in.Position);
    float distance = length(fs_in.lightVector.xyz - fs_in.Position);
    attenuation = 1.0 / (1.0 + 0.027 * distance +  0.0028 * (distance * distance));
  }
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

  vec4 diffuseTextureColor = texture(diffuseTexture, fs_in.TextureCoordinate);
  vec4 diffuseCubeTextureColor = texture(diffuseCubeTexture, fs_in.rawPosition);
  vec3 color = isCube == 1 ? diffuseCubeTextureColor.rgb : diffuseTextureColor.rgb;
  vec3 lighting = (ambient + attenuation * shadow * (diffuse + specular)) * color;
  FragColor = vec4(lighting, 1.0);
}
