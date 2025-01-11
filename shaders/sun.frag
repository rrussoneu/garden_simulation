#version 330 core
in vec3 Normal;
in vec3 FragPos;

uniform vec3 sunColor;    // Color based on temperature
uniform float intensity;  // Brightness based on temperature

out vec4 FragColor;

void main() {
    // Create a glowing effect by brightening edges

    // View direction
    vec3 viewDir = normalize(-FragPos);

    // Rim light factor
    float rimFactor = 1.0 - max(dot(normalize(Normal), viewDir), 0.0);

    // Mix base color with a brighter color at the edges
    vec3 glowColor = mix(sunColor, vec3(1.0), rimFactor * 0.5);

    // Apply intensity based on temperature
    vec3 finalColor = glowColor * intensity;

    // Output with full opacity
    FragColor = vec4(finalColor, 1.0);
}