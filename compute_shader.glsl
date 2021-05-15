#version 450 core

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba32f) uniform image2D imgOutput;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

float sphereHit(vec3 center, float radius, Ray ray, float tmin, float tmax)
{
    vec3 oc = ray.origin - center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
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

    // camera settings
    vec3 lowerLeftCorner = vec3(-2.0, -2.0, -1.0);
    vec3 horizontal      = vec3(4.0, 0.0, 0.0);
    vec3 vertical        = vec3(0.0, 4.0, 0.0);

    float u = float(pixelCoords.x) / dims.x;
    float v = float(pixelCoords.y) / dims.y;

    // Create a ray to each pixel of the output texture
    Ray ray = Ray(vec3(0), lowerLeftCorner + u * horizontal + v * vertical);

    vec3 sphereCenter  = vec3(0.0, 0.0, -5.0);
    float sphereRadius = 2.5;

    float tmin = 0.0;
    float tmax = FLT_MAX;

    float t1 = sphereHit(sphereCenter, sphereRadius, ray, tmin, tmax);

    vec3 sphereCenter2 = vec3(0.0, -502.5, -5.0);
    float sphereRadius2 = 500.0;

    if(t1 > 0.0)
    {
        tmax = t1;
    }

    float t2 = sphereHit(sphereCenter2, sphereRadius2, ray, tmin, tmax);

    if(t2 > 0.0)
    {
        vec3 normal2 = normalize(pointAtParameter(ray, t2) - sphereCenter2);
        vec4 pixel = vec4(getColor(t2, ray, normal2), 1.0);
        imageStore(imgOutput, pixelCoords, pixel);
    }
    else
    {
        vec3 normal1 = normalize(pointAtParameter(ray, t1) - sphereCenter);
        vec4 pixel = vec4(getColor(t1, ray, normal1), 1.0);
        imageStore(imgOutput, pixelCoords, pixel);
    }
}
