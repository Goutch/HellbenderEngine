#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;

layout(location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
} primaryRayPayload;

layout(location = 1) rayPayloadEXT bool isOccluded;


const vec3 TO_LIGHT_DIR = vec3(0, 1, 0.0);
const float AMBIENT_LIGHT_INTENSITY=0.15;
float random (vec2 coord) {
    return fract(sin(dot(coord.xy,
    vec2(12.9898, 78.233)))*
    43758.5453123);
}
vec3 RandDirHemisphere(vec3 dir)
{
    return vec3(0, 0, 0);
}
bool occlusionRay(vec3 origin, vec3 dir)
{
    const float tmin = 0.001;
    const float tmax = 1000.0;
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
    1);//payload location
    return isOccluded;
}

void main()
{
    primaryRayPayload.color = vec3(1, 1, 1);
    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    // Basic lighting
    vec3 toLightDir = normalize(TO_LIGHT_DIR);

    bool isInShadow = occlusionRay(origin, toLightDir);
    float light = AMBIENT_LIGHT_INTENSITY;
    if (!isInShadow) {
        light =  clamp(mix(AMBIENT_LIGHT_INTENSITY, 1, dot(toLightDir, hitResult.normal)), 0, 1);
    }
    primaryRayPayload.color *= light;

    /*float occlusion = 0;
    for (int i = 0; i < 16; i++)
    {
        bool o =occlusionRay(origin, RandDirHemisphere(hitResult.normal));
        occlusion += (o?1.0:0.0)/16.0f;
    }
    occlusion=1.0-occlusion;
    hitValue*=occlusion;
    */


}
