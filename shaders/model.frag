#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseMap;
uniform bool hasDiffuseMap;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // Get base color either from texture or material properties
    vec3 baseColor;
    if (hasDiffuseMap) {
        baseColor = texture(diffuseMap, TexCoords).rgb;
    } else {
        baseColor = material.diffuse;
    }

    // Calculate lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // Combine ambient and diffuse lighting
    vec3 ambient = lightColor * material.ambient * baseColor;
    vec3 diffuse = lightColor * diff * baseColor;

    // Final color
    FragColor = vec4(ambient + diffuse, 1.0);
}