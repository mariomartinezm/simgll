#version 430 core

in vec2 TexCoord;

out vec4 color;

uniform usampler2D sampler;

void main()
{
    uvec4 texColor = texture(sampler, TexCoord);

    if(texColor.r == 0)
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        color = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
