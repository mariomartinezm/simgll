#version 450 core

#define FLT_MAX 3.402823466e+38

layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba32f) uniform image2D imgOutput;

uniform uint cpuSeed;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere
{
    vec3 center;
    float radius;
    float t;
};

uint rand_pcg(inout uint rng_state)
{
    uint state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

vec3 randomUnitVector(inout uint rng_state)
{
    vec3 p = vec3(float(rand_pcg(rng_state)) / 4294967296.0,
                  float(rand_pcg(rng_state)) / 4294967296.0,
                  float(rand_pcg(rng_state)) / 4294967296.0);

    p = 2.0 * p - vec3(1.0);
    p = normalize(p);

    return p;
}

float sphereHit(Sphere sphere, Ray ray, float tmin, float tmax)
{
    vec3 oc = ray.origin - sphere.center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;
    float t;

    if(discriminant > 0)
    {
        t = (-b - sqrt(discriminant)) / (2.0 * a);

        if(t > tmin && t < tmax)
        {
            return t;
        }

        t = (-b + sqrt(discriminant)) / (2.0 * a);

        if(t > tmin && t < tmax)
        {
            return t;
        }
    }

    return -1.0;
}

vec3 pointAtParameter(Ray ray, float t)
{
    return ray.origin + t * ray.direction;
}

vec3 getColor(float t, Ray ray, vec3 normal)
{
    if(t > 0.0)
    {
        return 0.5 * vec3(normal.x + 1, normal.y + 1, normal.z + 1);
    }

    vec3 dirNormalized = normalize(ray.direction);
    t = 0.5 * (dirNormalized.y + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main()
{
    ivec2 dims = imageSize(imgOutput);

    // get index in global work group, i.e., x, y position
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    uint seed = (dims.x * gl_GlobalInvocationID.y + gl_GlobalInvocationID.x) * cpuSeed;

    // camera settings
    vec3 lowerLeftCorner = vec3(-2.0, -2.0, -1.0);
    vec3 horizontal      = vec3(4.0, 0.0, 0.0);
    vec3 vertical        = vec3(0.0, 4.0, 0.0);

    Sphere spheres[2];
    spheres[0] = Sphere(vec3(0.0, 0.0, -5.0), 2.5, -1.0);
    spheres[1] = Sphere(vec3(0.0, -502.5, -5.0), 500, -1.0);

    // Initialize color of the hitpoint
    vec4 pixel = vec4(vec3(0.0), 1.0);

    for(int j = 0; j < 100; j++)
    {
        float u = float(pixelCoords.x + float(rand_pcg(seed)) / 4294967296.0) / (dims.x - 1);
        float v = float(pixelCoords.y + float(rand_pcg(seed)) / 4294967296.0) / (dims.y - 1);

        // Create a ray to each pixel of the output texture
        Ray ray = Ray(vec3(0), lowerLeftCorner + u * horizontal + v * vertical);

        float tmin = 0.0;
        float tmax = FLT_MAX;

        int hitIndex = 0;
        for(int i = 0; i < spheres.length(); i++)
        {
            spheres[i].t = sphereHit(spheres[i], ray, tmin, tmax);

            if(spheres[i].t > 0.0)
            {
                tmax = spheres[i].t;
                hitIndex = i;
            }
        }

        // Calculate the hit point's position and normal
        float t     = spheres[hitIndex].t;
        vec3 p      = pointAtParameter(ray, t);
        vec3 normal = normalize(p - spheres[hitIndex].center);

        tmax = FLT_MAX;

        vec3 factor = vec3(1.0);
        while(t > 0.0)
        {
            vec3 target = p + normal + randomUnitVector(seed);
            ray = Ray(p, target - p);

            hitIndex = 0;
            for(int i = 0; i < spheres.length(); i++)
            {
                spheres[i].t = sphereHit(spheres[i], ray, tmin, tmax);

                if(spheres[i].t > 0.0)
                {
                    tmax = spheres[i].t;
                    hitIndex = i;
                }
            }

            if(t > 0.0)
            {
                t      = spheres[hitIndex].t;
                p      = pointAtParameter(ray, t);
                normal = normalize(p - spheres[hitIndex].center);

                factor *= 0.5;
            }
        }

        pixel += vec4(0.01 * factor * getColor(-1, ray, normal), 1.0);
    }

    imageStore(imgOutput, pixelCoords, pixel);
}
