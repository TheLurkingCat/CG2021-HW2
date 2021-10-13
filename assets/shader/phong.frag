#version 330 core
layout(location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec4 baseColor;
uniform vec4 lightPos;
uniform vec4 viewPos;

void main() {
    vec3 color = baseColor.rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.65);
    // Ambient
    vec3 ambient = 0.25 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos.xyz);
    float normaldotlight = dot(normal, lightDir);
    float diff = max(normaldotlight, 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos.xyz - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * color;
    FragColor = vec4(lighting, 1.0);
}
