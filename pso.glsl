#version 450 core

layout (local_size_x = 16) in;

uniform float omega;
uniform vec3 bestPosition;

struct Particle
{
    vec3 position;
    vec3 velocity;
    vec3 bestPosition;
    float fitness;
};

layout (std430, binding = 0) readonly buffer inSwarm
{
    Particle particles[];
} inputData;

layout (std430, binding = 1) buffer outSwarm
{
    Particle particles[];
} outputData;

float goldsteinPrice(vec2 p)
{
    return (1  + (p.x + p.y + 1) * (p.x + p.y + 1) *
           (19 - 14 * p.x + 3 * p.x * p.x - 14 * p.y + 6 * p.x * p.y + 3 * p.y * p.y)) *
           (30 + (2 * p.x - 3 * p.y) * (2 * p.x - 3 * p.y) *
           (18 - 32 * p.x + 12 * p.x * p.x + 48 * p.y - 36 * p.x * p.y + 27 * p.y * p.y));
}

int random(int seed, int iterations)
{
    int value = seed;
    int n;

    for(n = 0; n < iterations; n++)
    {
        value = ((value >> 7) ^ (value << 9)) * 15485863;
    }

    return value;
}

void main()
{
    int globalId = int(gl_GlobalInvocationID.x);

    Particle pIn  = inputData.particles[globalId];
    Particle pOut;

    int r1 = random(globalId, 4);
    int r2 = random(r1, 2);

    float k1 = (r1 & 0x3FF) / 1024.0;
    float k2 = (r2 & 0x3FF) / 1024.0;

    pOut.velocity = omega * pIn.velocity +
        2.0 * k1 * (pIn.bestPosition - pIn.position) +
        2.0 * k2 * (bestPosition - pIn.position);

    float speed = length(pOut.velocity);

    if(speed > 4)
    {
        pOut.velocity = 4 * normalize(pOut.velocity);
    }

    pOut.position = pIn.position + pOut.velocity;
    pOut.fitness  = goldsteinPrice(pOut.position.xy);

    if(pOut.fitness < pIn.fitness)
    {
        pOut.bestPosition = pOut.position;
    }
    else
    {
        pOut.bestPosition = pIn.bestPosition;
    }

    outputData.particles[globalId] = pOut;
}
