#version 450 core

layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba32f) uniform image2D imgOutput;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

float sphereHit(vec3 center, float radius, Ray ray)
{
    vec3 oc = ray.origin - center;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if(discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

vec3 pointAtParameter(Ray ray, float t)
{
    return ray.origin + t * ray.direction;
}

vec3 getColor(Ray ray)
{
    vec3 sphereCenter  = vec3(0.0, 0.0, -5.0);
    float sphereRadius = 3.0;

    float t = sphereHit(sphereCenter, sphereRadius, ray);

    if(t > 0.0)
    {
        vec3 normal = normalize(pointAtParameter(ray, t) - sphereCenter);

        return 0.5 * vec3(normal.x + 1, normal.y + 1, normal.z + 1);
    }

    vec3 normDirection = normalize(ray.direction);
    t = 0.5 * (normDirection.y + 1.0);
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

    vec4 pixel = vec4(getColor(ray), 1.0);

    imageStore(imgOutput, pixelCoords, pixel);
}
