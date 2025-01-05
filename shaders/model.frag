#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseMap;
uniform bool hasDiffuseMap;
uniform bool isPreview;
uniform float previewAlpha;
uniform vec3 previewColor;

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
    if (isPreview) {
        // Preview rendering - simple lighting with highlight color
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        // Use preview color with basic lighting
        vec3 color = previewColor * (0.5 + 0.5 * diff);
        FragColor = vec4(color, previewAlpha);
    } else {
        // Normal rendering - full material and texture
        vec3 baseColor = hasDiffuseMap ? texture(diffuseMap, TexCoords).rgb : material.diffuse;

        // Calculate lighting
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        // Combine lighting
        vec3 ambient = lightColor * material.ambient * baseColor;
        vec3 diffuse = lightColor * diff * baseColor;

        FragColor = vec4(ambient + diffuse, 1.0);
    }
}