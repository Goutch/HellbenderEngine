#version 460
#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int depth;
    int sample_id;
    float hit_t;
} primaryRayPayload;
layout (binding = 3, set = 0) uniform TimeUBO
{
    float value;
} time;
const float E =  2.71828182845904523536028747135266249;

void main()
{
    vec3 position = gl_WorldRayOriginEXT + (gl_WorldRayDirectionEXT * gl_RayTmaxEXT);
    position.y+=500;
    float ratio = (((position.y/2)+(gl_RayTmaxEXT/2))/gl_RayTmaxEXT);
    vec3 colorValue = vec3(1-ratio, 0.3, ratio * 0.5);
    vec3 toLightDir = normalize(vec3(sin(time.value), sin(time.value), cos(time.value)));
    float sunValue = max(pow(dot(toLightDir, gl_WorldRayDirectionEXT), E*30), 0);
    colorValue += vec3(sunValue, sunValue*0.5, sunValue*0.2)*20.0;

    primaryRayPayload.depth++;
    primaryRayPayload.color = colorValue* (2.0 / (primaryRayPayload.depth));
    primaryRayPayload.hit_t = gl_RayTmaxEXT;
}