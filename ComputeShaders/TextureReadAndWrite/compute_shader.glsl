#version 430 core

layout (local_size_x = 32, local_size_y = 32) in;
layout (binding = 0, rgba32f) uniform image2D imgInput;
layout (binding = 1, rgba32f) uniform image2D imgOutput;

void main()
{
    vec4 texel;

    ivec2 p = ivec2(gl_GlobalInvocationID.xy);

    texel = imageLoad(imgInput, p);
    texel = vec4(1.0) - texel;
    imageStore(imgOutput, p, texel);
}
