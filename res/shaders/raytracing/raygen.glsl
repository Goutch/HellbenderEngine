#version 460
#extension GL_EXT_ray_tracing : enable
#define HISTORY_COUNT 8
layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D outputImage;
layout(binding = 2, set = 0, rgba32f) uniform image2D history[HISTORY_COUNT];
layout(binding = 3, set = 0) uniform CameraProperties
{
    mat4 viewInverse;
    mat4 projInverse;
} cam;


layout(location = 0) rayPayloadEXT PrimaryRayPayLoad
{
    vec3 color;
} primaryRayPayload;

void main()
{
    const vec2 pixelCenter = vec2(gl_LaunchIDEXT.xy) + vec2(0.5);
    const vec2 inUV = pixelCenter/vec2(gl_LaunchSizeEXT.xy);
    vec2 d = inUV * 2.0 - 1.0;

    vec4 origin = cam.viewInverse * vec4(0, 0, 0, 1);
    vec4 target = cam.projInverse * vec4(d.x, d.y, 1, 1);
    vec4 direction = cam.viewInverse*vec4(normalize(target.xyz), 0);

    float tmin = 0.001;
    float tmax = 1000.0;

    primaryRayPayload.color = vec3(0.0);
    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xff, 0, 0, 0, origin.xyz, tmin, direction.xyz, tmax, 0);

    imageStore(outputImage, ivec2(gl_LaunchIDEXT.xy), vec4(primaryRayPayload.color, 0.0));
}
