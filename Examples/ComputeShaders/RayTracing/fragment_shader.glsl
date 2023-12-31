#version 450 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D sampler;

void main()
{
    vec4 texColor = texture(sampler, TexCoord);

    color = texColor;
}
