#version 460 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
    vec3 texCol = texture(tex, TexCoord).rgb;
    color = vec4(texCol, 1.0);
}
