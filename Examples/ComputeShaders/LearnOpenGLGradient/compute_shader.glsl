#version 460 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout (location = 0) uniform float time;

void main()
{
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    float speed = 100;
    float width = 1000;

    /* Uncomment these two lines and comment the following two if the local
     * size is set to 1 in each dimension and the number of work groups
     * corresponds to the dimensions of the texture */
    /*value.x = mod(float(texelCoord.x) + time * speed, width) / (gl_NumWorkGroups.x);*/
    /*value.y = float(texelCoord.y) / (gl_NumWorkGroups.y);*/
    value.x = mod(float(texelCoord.x) + time * speed, width) /
                (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    value.y = float(texelCoord.y) /
                (gl_NumWorkGroups.y * gl_WorkGroupSize.y);

    imageStore(imgOutput, texelCoord, value);
}
