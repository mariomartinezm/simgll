#version 430 core

layout (location = 0) in vec4 position;

out float intensity;

uniform mat4 mvp;

void main()
{
    intensity = position.w;
    gl_Position = mvp * vec4(position.xyz, 1.0);
}
