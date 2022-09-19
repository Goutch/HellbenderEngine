#version 460
#extension GL_EXT_ray_tracing: enable
#extension GL_EXT_nonuniform_qualifier: enable

hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout (binding = 3, set = 0) uniform TimeUBO
{
    float value;
} time;
layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
} primaryRayPayload;

layout (location = 1) rayPayloadEXT bool isOccluded;


const float GOLDEN_RATIO = 1.61803;
const vec3 TO_LIGHT_DIR = vec3(0, 1, 0.0);
const float AMBIENT_LIGHT_INTENSITY = 0.15;
float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio
float random(vec3 pos) {
    pos = vec3(mod(pos.x, 1.0),mod(pos.y, 1.0),mod(pos.z, 1.0));
    return fract(sin(dot(pos, vec3(64.25375463, 23.27536534, 86.29678483))) * 59482.7542);
}
float gold_noise(in vec2 xy, in float seed) {
    return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}
float random(vec2 coord) {
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}
vec3 RandDirHemisphere(vec3 normal, vec3 coord)
{
    vec3 dir = vec3(random(coord) - 0.5, random(coord+vec3(PHI)) - 0.5, random(coord+vec3(PHI*2)) - 0.5);
    dir = normalize(dir);
    if (dot(normal, dir) < 0.0)
    {
        dir = -dir;
    }
    return dir;
}
bool occlusionRay(vec3 origin, vec3 dir, float tmax)
{
    const float tmin = 0.001;
    isOccluded = true;
    // Trace shadow ray and offset indices to match shadow hit/miss shader group indices
    traceRayEXT(topLevelAS, //topLevelacceleationStructure
                gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, //rayFlags
                0xFF, //cullMask
                0, //sbtRecordOffset
                1, //sbtRecordStride
                1, //missIndex
                origin, //origin
                tmin, //Tmin
                dir, //direction
                tmax, //Tmax
                1); //payload location
    return isOccluded;
}

void main()
{
    primaryRayPayload.color = vec3(1, 1, 1);
    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    // Basic lighting
    vec3 toLightDir = normalize(TO_LIGHT_DIR);

    bool isInShadow = occlusionRay(origin, toLightDir, 1000.0);
    float light = AMBIENT_LIGHT_INTENSITY;
    if (!isInShadow) {
        light = clamp(mix(AMBIENT_LIGHT_INTENSITY, 1, dot(toLightDir, hitResult.normal)), 0, 1);
    }


    float occlusion = 0;
    const int numSamples = 8;
    for (int i = 0; i < numSamples; i++)
    {
        vec2 coord = vec2(gl_LaunchIDEXT.xy) / vec2(gl_LaunchSizeEXT.xy);
        vec3 pos = vec3(coord.x, coord.y, time.value + (i * PHI));
        bool o = occlusionRay(origin, RandDirHemisphere(hitResult.normal, pos),0.3);
        occlusion += float(o) / float(numSamples);
    }
    light *= 1.0 - occlusion;

    primaryRayPayload.color *= light;


}
