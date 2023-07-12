#version 460 core

layout(std430) buffer;
layout(local_size_x = 1024) in;

layout(binding = 0) coherent readonly buffer Input0
{
    vec4 elements[];
} input_dataA;

layout(binding = 1) coherent readonly buffer Input1
{
    vec4 elements[];
} input_dataB;

layout(binding = 2) coherent writeonly buffer Output
{
    vec4 elements[];
} output_data;

void main()
{
    uint id = gl_GlobalInvocationID.x;
    output_data.elements[id] = input_dataA.elements[id] *
        input_dataB.elements[id];
}
