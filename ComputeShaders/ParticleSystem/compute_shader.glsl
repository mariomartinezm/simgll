#version 430 core

layout (std140, binding = 0) uniform attractor_block
{
    vec4 attractor[64]; // xyz = position, w = mass
};

// Process particles in blocks of 128
layout (local_size_x = 128) in;

// Buffers containing the position and velocities of the particles
layout (rgba32f, binding = 0) uniform imageBuffer positionBuffer;
layout (rgba32f, binding = 1) uniform imageBuffer velocityBuffer;

// Delta time
uniform float dt = 1.0;

void main()
{
    // Read the current position and velocity from the buffers
    vec4 vel = imageLoad(velocityBuffer, int(gl_GlobalInvocationID.x));
    vec4 pos = imageLoad(positionBuffer, int(gl_GlobalInvocationID.x));

    int i;

    // Update position using current velocity * time
    pos.xyz += vel.xyz * dt;

    // Update "life" of particle in w component
    pos.w -= 0.0001 * dt;

    // For each attractor ...
    for(i = 0; i < 4; i++)
    {
        // Calculate force and update velocity accordingly
        vec3 dist = (attractor[i].xyz - pos.xyz);
        vel.xyz += dt * dt * attractor[i].w * normalize(dist) / (dot(dist, dist) + 10.0);
    }

    // If the particle expires, reset it
    if(pos.w <= 0.0)
    {
        pos.xyz  = -pos.xyz * 0.01;
        vel.xyz *= 0.01;
        pos.w   += 1.0f;
    }

    // Store the new position and velocity back into the buffers
    imageStore(positionBuffer, int(gl_GlobalInvocationID.x), pos);
    imageStore(velocityBuffer, int(gl_GlobalInvocationID.x), vel);
}
