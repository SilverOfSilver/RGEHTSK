#version 330

// Color inputs from Raylib
in vec4 fragColor;
in vec2 fragTexCoord;

// Custom vertex colors
uniform vec3 v1Color;
uniform vec3 v2Color;
uniform vec3 v3Color;
uniform vec2 v1Pos;
uniform vec2 v2Pos;
uniform vec2 v3Pos;

out vec4 finalColor;

void main() {
    // Barycentric coordinates for smooth color interpolation
    vec3 bary = vec3(
        length(fragTexCoord - v2Pos) + length(fragTexCoord - v3Pos),
        length(fragTexCoord - v1Pos) + length(fragTexCoord - v3Pos),
        length(fragTexCoord - v1Pos) + length(fragTexCoord - v2Pos)
    );

    bary = 1.0 - bary / (bary.x + bary.y + bary.z);

    // Interpolate the colors
    vec3 color = bary.x * v1Color + bary.y * v2Color + bary.z * v3Color;
    finalColor = vec4(color, 1.0);
}
