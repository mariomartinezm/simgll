#version 450 core

in vec4 Color;
out vec4 color;

uniform vec3 guiColor;

void main()
{
    color = vec4(guiColor * Color.rgb, 1.0);
}
