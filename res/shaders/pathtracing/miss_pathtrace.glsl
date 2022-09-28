#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : require
#include "common.glsl"

layout(location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int depth;
    uint rng_state;
    bool hit_sky;
} primaryRayPayload;

const float E =  2.71828182845904523536028747135266249;

void main()
{
    vec3 position = gl_WorldRayOriginEXT + (gl_WorldRayDirectionEXT * gl_RayTmaxEXT);
    position.y+=500;


    float ratio = (((position.y/2)+(gl_RayTmaxEXT/2))/gl_RayTmaxEXT);
    vec3 sky_color = vec3(1-ratio, 0.3, ratio * 0.5);

    vec3 toLightDir = normalize(vec3(sin(frame.time), sin(frame.time), cos(frame.time)));
    float sun_emision = max(pow(dot(toLightDir, gl_WorldRayDirectionEXT), E*30), 0);
    vec3 sunColor =vec3(sun_emision, sun_emision*0.6, sun_emision*0.2);


    primaryRayPayload.depth++;
    primaryRayPayload.color = ((sunColor*10.0)+(sky_color))/primaryRayPayload.depth;
    primaryRayPayload.depth--;
    primaryRayPayload.hit_sky = true;
}