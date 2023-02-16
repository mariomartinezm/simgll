#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 Color;

uniform float rotation;
uniform vec2 translation;

void main()
{
    Color = color;

    vec2 rotatedPos;
    rotatedPos.x = translation.x + position.x * cos(rotation) - position.y * sin(rotation);
    rotatedPos.y = translation.y + position.x * sin(rotation) + position.y * cos(rotation);

    gl_Position = vec4(rotatedPos, position.z, 1.0);
}
