#version 450 core

const float FLT_MAX          = 3.402823466e+38;
const float INT_MAX          = 4294967296.0;
const float MIN_REFLECTION   = 0.01;
const float FUZZ_FACTOR      = 0;
const float REFRACTION_INDEX = 1.5;
const int   NUM_SPHERES      = 5;
const int   NUM_SAMPLES      = 100;

// Materials
const uint LAMBERTIAN = 0;
const uint METAL      = 1;
const uint DIELECTRIC = 2;

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
    vec3  center;
    float radius;
    float t;
    vec3  albedo;
    uint  material;
};

struct Hit
{
    float t;
    vec3  p;
    vec3  normal;
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
    vec3 p = vec3(float(rand_pcg(rng_state)) / INT_MAX,
                  float(rand_pcg(rng_state)) / INT_MAX,
                  float(rand_pcg(rng_state)) / INT_MAX);

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

int worldHit(inout Sphere spheres[NUM_SPHERES], Ray ray)
{
    // Some of the rays do not hit at exactly 0, so we compensate
    float tmin = 0.001;
    float tmax = FLT_MAX;

    int hitIndex = -1;
    for(int i = 0; i < NUM_SPHERES; i++)
    {
        spheres[i].t = sphereHit(spheres[i], ray, tmin, tmax);

        if(spheres[i].t > 0.0)
        {
            tmax = spheres[i].t;
            hitIndex = i;
        }
    }

    return hitIndex;
}

vec3 pointAtParameter(Ray ray, float t)
{
    return ray.origin + t * ray.direction;
}

vec3 getColor(Ray ray)
{
    vec3 dirNormalized = normalize(ray.direction);
    float t = 0.5 * (dirNormalized.y + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

bool nearZero(vec3 v)
{
    // Return true if the vector is close to zero in all dimensions
    const float s = 1e-8;
    return (abs(v.x) < s) && (abs(v.y) < s) && (abs(v.z) < s);
}

// Schlick's approximation for reflectance
float reflectance(float cosine, float refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);

    r0 *= r0;

    return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool scatter(Hit hit, uint seed, uint material, inout Ray scattered)
{
    switch(material)
    {
        case LAMBERTIAN:
            vec3 scatterDirection = hit.normal + randomUnitVector(seed);

            // Catch degerate scatter direction
            if(nearZero(scatterDirection))
            {
                scatterDirection = hit.normal;
            }

            scattered = Ray(hit.p, scatterDirection);
            return true;
            break;

        case METAL:
            vec3 reflected = reflect(normalize(scattered.direction), hit.normal);
            float fuziness = FUZZ_FACTOR < 1 ? FUZZ_FACTOR : 1.0;
            scattered = Ray(hit.p, reflected + fuziness * randomUnitVector(seed));
            return (dot(scattered.direction, hit.normal) > 0);
            break;

        case DIELECTRIC:
            vec3 outwardNormal;
            float niOverNt;

            // Determine which side of the sphere the ray is on
            if(dot(scattered.direction, hit.normal) > 0)
            {
                // Ray is inside the sphere
                outwardNormal = -hit.normal;
                niOverNt      = REFRACTION_INDEX;
            }
            else
            {
                // Rays is outside the sphere
                outwardNormal = hit.normal;
                niOverNt      = 1.0 / REFRACTION_INDEX;
            }

            vec3 unitDirection = normalize(scattered.direction);

            float cosTheta = min(dot(-unitDirection, outwardNormal), 1.0);
            float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

            bool cannotRefract = (niOverNt * sinTheta) > 1.0;

            vec3 direction;
            if(cannotRefract ||
               reflectance(cosTheta, REFRACTION_INDEX) > float(rand_pcg(seed)) / INT_MAX)
            {
                direction = reflect(unitDirection, hit.normal);
            }
            else
            {
                direction = refract(unitDirection, outwardNormal, niOverNt);
            }

            scattered = Ray(hit.p, direction);

            return true;
            break;

        default:
            return true;
            break;
    }
}

void main()
{
    ivec2 dims = imageSize(imgOutput);

    // get index in global work group, i.e., x, y position
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    uint seed = (dims.x * gl_GlobalInvocationID.y + gl_GlobalInvocationID.x) * cpuSeed;

    // camera settings
    vec3 lowerLeftCorner = vec3(-2.0, -2.0, -1.0);
    vec3 horizontal      = vec3( 4.0,  0.0,  0.0);
    vec3 vertical        = vec3( 0.0,  4.0,  0.0);

    Sphere spheres[NUM_SPHERES];
    spheres[0] = Sphere(vec3( 0.0,    0.0, -5.0), 2.5, -1.0, vec3(0.8, 0.3, 0.3), LAMBERTIAN);
    spheres[1] = Sphere(vec3( 0.0, -502.5, -5.0), 500, -1.0, vec3(0.3, 0.3, 0.0), LAMBERTIAN);
    spheres[2] = Sphere(vec3(-4.5,    0.0, -5.0), 2.5, -1.0, vec3(1.0, 1.0, 1.0), DIELECTRIC);
    spheres[3] = Sphere(vec3(-4.5,    0.0, -5.0),-2.4, -1.0, vec3(1.0, 1.0, 1.0), DIELECTRIC);
    spheres[4] = Sphere(vec3( 4.5,    0.0, -5.0), 2.5, -1.0, vec3(0.8, 0.6, 0.2), METAL);

    // Initialize color of the hitpoint
    vec4 pixel = vec4(vec3(0.0), 1.0);

    for(int j = 0; j < NUM_SAMPLES; j++)
    {
        // Randomly create coordinates for the sample
        float u = float(pixelCoords.x + float(rand_pcg(seed)) / INT_MAX) / (dims.x - 1);
        float v = float(pixelCoords.y + float(rand_pcg(seed)) / INT_MAX) / (dims.y - 1);

        // Create a ray to sample of the output texture
        Ray ray = Ray(vec3(0), lowerLeftCorner + u * horizontal + v * vertical);
        int hitIndex = worldHit(spheres, ray);

        // Initialize the reflection factor
        vec3 reflection = vec3(1.0);

        while(hitIndex >= 0)
        {
            // This is a hit so we decrease reflection
            reflection *= spheres[hitIndex].albedo;

            // Calculate the hit point's position and normal
            Hit hit;
            hit.t      = spheres[hitIndex].t;
            hit.p      = pointAtParameter(ray, hit.t);
            hit.normal = normalize(hit.p - spheres[hitIndex].center);

            // Calculate scattering and send a ray along this direction
            bool s = scatter(hit, seed, spheres[hitIndex].material, ray);

            // Did we hit anything?
            hitIndex = worldHit(spheres, ray);

            // If reflection is too small or if there is no scattering just
            // break
            if(length(reflection) < MIN_REFLECTION || (!s))
            {
                break;
            }
        }

        // Add contribution for this sample
        pixel += vec4((1.0 / NUM_SAMPLES) * sqrt(reflection * getColor(ray)), 1.0);
    }

    imageStore(imgOutput, pixelCoords, pixel);
}
