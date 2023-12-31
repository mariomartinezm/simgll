#version 430

layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba8ui) uniform uimage2D imgInput;
layout(binding = 1, rgba8ui) uniform uimage2D imgOutput;

uniform uint cpuSeed;

uvec2 tea(uvec2 v, int rounds)
{
    const uvec4 key = {0xa341316c, 0xc8013ea4, 0xad90777d, 0x7e95761e};
    const uint delta = 0x9e3779b9;
    uint sum = 0;

    for(int i = 0; i < rounds; i++)
    {
        sum += delta;
        v[0] += ((v[1] << 4) + key[0]) ^ (v[1] + sum) ^ ((v[1] >> 5) + key[1]);
        v[1] += ((v[0] << 4) + key[2]) ^ (v[0] + sum) ^ ((v[0] >> 5) + key[3]);
    }

    return v;
}

void main()
{
    const int nRounds = 5;
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uvec4 state;

    uvec2 seed = gl_GlobalInvocationID.xy * cpuSeed;
    uvec2 rand = tea(seed, nRounds);
    float p = float(rand[0]) / 4294967296.0;

    if(p < 0.5)
    {
        state = uvec4(0, 0, 0, 255);
    }
    else
    {
        state = uvec4(1, 0, 0, 255);
    }

    imageStore(imgOutput, coord, state);
}
