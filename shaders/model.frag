#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform bool hasDiffuseMap;
uniform bool hasNormalMap;

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
    // Get diffuse color either from texture or material
    vec3 diffuseColor = hasDiffuseMap ? texture(diffuseMap, TexCoords).rgb : material.diffuse;

    // Calculate normal
    vec3 norm = normalize(Normal);
    if (hasNormalMap) {
        vec3 normalMap = texture(normalMap, TexCoords).rgb * 2.0 - 1.0;
        norm = normalize(norm + normalMap);
    }

    // Ambient
    vec3 ambient = material.ambient * diffuseColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * diffuseColor);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular * 0.3);

    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}