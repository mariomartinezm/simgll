#version 450 core

layout(local_size_x = 32, local_size_y = 32) in;
layout(binding = 0, rgba32f) uniform image2D imgOutput;

vec3 getBackgroundColor(vec3 rayDirection)
{
    vec3 normDirection = normalize(rayDirection);
    float t = 0.5 * (normDirection.y + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

bool isSphereHit(vec3 center, float radius, vec3 rayOrigin, vec3 rayDirection)
{
    vec3 oc = rayOrigin - center;

    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(oc, rayDirection);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    return (discriminant > 0);
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

    vec3 rayOrigin    = vec3(0.0, 0.0, 0.0);
    vec3 rayDirection = lowerLeftCorner + u * horizontal + v * vertical;

    vec3 sphereCenter  = vec3(0.0, 0.0, -5.0);
    float sphereRadius = 3.0;

    vec4 pixel = vec4(getBackgroundColor(rayDirection), 1.0);

    if(isSphereHit(sphereCenter, sphereRadius, rayOrigin, rayDirection))
    {
        pixel = vec4(1.0, 0.0, 0.0, 1.0);
    }

    imageStore(imgOutput, pixelCoords, pixel);
}
