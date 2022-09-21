#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 1) rayPayloadInEXT OcclusionRayPayload
{
    float occlusion;
    float MAX_T;
} occlusionRayPayload;

void main()
{
    occlusionRayPayload.occlusion = 0.0;
}