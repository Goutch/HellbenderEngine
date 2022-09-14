#version 460
#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadInEXT vec3 hitValue;

void main()
{
    vec3 position = gl_WorldRayOriginEXT + (gl_WorldRayDirectionEXT * gl_RayTmaxEXT);
    position.y+=750;
    float ratio = (((position.y/2)+(gl_RayTmaxEXT/2))/gl_RayTmaxEXT);
    vec3 colorValue = vec3(1-ratio, 0.3, ratio * 0.5);
    hitValue = colorValue;
}