#version 330 core
out vec4 FragColor;

uniform vec3 gridColor;

void main() {
    // Simple solid color for grid, ignoring lighting
    FragColor = vec4(0.7, 0.5, 0.3, 1.0);  // Brown color for soil
}