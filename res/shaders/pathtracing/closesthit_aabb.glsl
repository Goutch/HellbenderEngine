#version 460
#extension GL_EXT_ray_tracing: enable
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_GOOGLE_include_directive : require
#include "common.glsl"
//------------------------------------ UNIFORMS ------------------------------------



hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;


#include "pathtrace.glsl"
void main()
{
    vec3 normal = hitResult.normal;
    traceRays(normal);
}

