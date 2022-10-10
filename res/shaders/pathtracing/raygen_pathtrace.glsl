#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier :require
#define HISTORY_COUNT 8
#include "common.glsl"

struct Vertex{
    vec4 pos;
};

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D history[HISTORY_COUNT];
layout(binding = 2, set = 0, rgba32f) uniform writeonly image2D outputImage;
layout(binding = 3, set = 0) uniform CameraProperties
{
    mat4 viewInverse;
    mat4 projInverse;
} cam;
//layout(binding = 6, set = 0) buffer VertexBuffer
//{
//    Vertex vertices[];
//} vertices[];

layout(location = 0) rayPayloadEXT PrimaryRayPayLoad
{
    vec3 color;
    int bounce_count;
    uint rng_state;
    bool hit_sky;
    vec3 hit_normal;
    float hit_t;
} primaryRayPayload;

vec2 offsets[16] =
{
vec2(0.500000, 0.333333),
vec2(0.250000, 0.666667),
vec2(0.750000, 0.111111),
vec2(0.125000, 0.444444),
vec2(0.625000, 0.777778),
vec2(0.375000, 0.222222),
vec2(0.875000, 0.555556),
vec2(0.062500, 0.888889),
vec2(0.562500, 0.037037),
vec2(0.312500, 0.370370),
vec2(0.812500, 0.703704),
vec2(0.187500, 0.148148),
vec2(0.687500, 0.481481),
vec2(0.437500, 0.814815),
vec2(0.937500, 0.259259),
vec2(0.031250, 0.592593),
};

void main()
{
    const vec2 pixelCenter = vec2(gl_LaunchIDEXT.xy) + vec2(0.5);
    const vec2 inUV = pixelCenter/vec2(gl_LaunchSizeEXT.xy);
    vec2 d = inUV * 2.0 - 1.0;

    vec2 offset=offsets[int(mod(frame.index, 16))];
    offset.x = ((offset.x-0.5f) / gl_LaunchSizeEXT.x) *2;
    offset.y = ((offset.y-0.5f) / gl_LaunchSizeEXT.y) *2;

    vec4 origin = cam.viewInverse* vec4(0, 0, 0, 1);
    vec4 target = cam.projInverse * vec4(d.x+offset.x, d.y+offset.y, 1, 1);

    vec4 direction = normalize(cam.viewInverse*vec4(normalize(target.xyz), 0));

    float tmin = 0.001;
    float tmax = 1000.0;


    vec3 color=vec3(0.0, 0.0, 0.0);
    int numSamples=1;

    float x = frame.index*PHI;
    x=mod(x, 1.0);
    x*=1000;

    primaryRayPayload.rng_state = uint(uint(gl_LaunchIDEXT.x) * uint(1973) + uint(gl_LaunchIDEXT.y) * uint(9277) + uint(floor(x)) * uint(26699)) | uint(1);
    for (int i=0; i<numSamples; i++)
    {

        primaryRayPayload.bounce_count = 0;
        primaryRayPayload.color = vec3(0.0, 0.0, 0.0);
        primaryRayPayload.hit_sky = false;
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
        color += primaryRayPayload.color;
    }
    imageStore(history[0], ivec2(gl_LaunchIDEXT.xy), vec4(color, 0.0));

    for (int i=1; i<HISTORY_COUNT; i++)
    {
        color += imageLoad(history[i], ivec2(gl_LaunchIDEXT.xy)).rgb;
    }

    color /= float(HISTORY_COUNT);
    const float gamma = 2.0;
    float exposure = 0.5;
    //tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    //exposure
    mapped = pow(mapped, vec3(1.0 / gamma));

    //mapped = vertices[nonuniformEXT(gl_LaunchIDEXT.x)].vertices[gl_LaunchIDEXT.y].pos.rgb;

    imageStore(outputImage, ivec2(gl_LaunchIDEXT.xy), vec4(color, 0.0));
}
