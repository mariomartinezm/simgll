#version 450 core

layout (local_size_x = 16) in;

uniform float omega;
uniform vec3 bestPosition;
uniform uint cpuSeed;

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
    int globalId = int(gl_GlobalInvocationID.x);

    Particle pIn  = inputData.particles[globalId];
    Particle pOut;

    uvec2 seed = gl_GlobalInvocationID.xy * cpuSeed;
    uvec2 rand = tea(seed, 5);

    float k1 = float(rand[0]) / 4294967296.0;
    float k2 = float(rand[1]) / 4294967296.0;

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
