#version 430 core

in float intensity;

out vec4 color;

void main()
{
    color = mix(vec4(0.0, 0.2, 1.0, 1.0), vec4(0.2, 0.05, 0.0, 1.0), intensity);
}
