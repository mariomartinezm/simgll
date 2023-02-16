#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 particle_position;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position + particle_position, 1.0);
}
