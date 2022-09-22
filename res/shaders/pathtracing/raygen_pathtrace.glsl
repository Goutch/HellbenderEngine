#version 460
#extension GL_EXT_ray_tracing : enable

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D image;
layout(binding = 2, set = 0) uniform CameraProperties
{
    mat4 viewInverse;
    mat4 projInverse;
} cam;

layout(location = 0) rayPayloadEXT PrimaryRayPayLoad
{
    vec3 color;
    int depth;
    int sample_id;
    float hit_t;
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

    int num_samples = 32;

    vec3 color=vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < num_samples; i++)
    {
        primaryRayPayload.sample_id=i;
        primaryRayPayload.depth = 0;
        primaryRayPayload.color = vec3(0.0, 0., 0.);
        primaryRayPayload.hit_t = 0.0;
        traceRayEXT(topLevelAS, //topLevelacceleationStructure
        gl_RayFlagsOpaqueEXT, //rayFlags
        0xff, //cullMask
        0, //sbtRecordOffset
        1, //sbtRecordStride
        0, //missIndex
        origin.xyz, //origin
        tmin, //Tmin
        direction.xyz, //direction
        tmax, //Tmax
        0);//payload location
        color += (primaryRayPayload.color/primaryRayPayload.depth)/num_samples;
    }

    imageStore(image, ivec2(gl_LaunchIDEXT.xy), vec4(color, 0.0));
}
