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

struct Material {
    vec3 albedo;
};
layout (binding = 4, set = 0) buffer Materials
{
    Material materials[];
} materials;

layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
} primaryRayPayload;

layout (location = 1) rayPayloadEXT bool isOccluded;


const float GOLDEN_RATIO = 1.61803;
const float AMBIENT_LIGHT_INTENSITY = 0.15;
float PHI = 1.61803398874989484820459;// Φ = Golden Ratio
float random(vec3 pos) {
    pos = vec3(mod(pos.x, 1.0), mod(pos.y, 1.0), mod(pos.z, 1.0));
    return fract(sin(dot(pos, vec3(64.25375463, 23.27536534, 86.29678483))) * 59482.7542);
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
    1);//payload location
    return isOccluded;
}

void main()
{
    vec3 material_color= vec3(0.5, 0.5, 0.5);

    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

    vec3 toLightDir = normalize(vec3(sin(time.value), sin(time.value), cos(time.value)));
    bool isInShadow = false;
    //if ((gl_LaunchIDEXT.x+gl_LaunchIDEXT.y)%2==0)
        isInShadow = occlusionRay(origin, toLightDir, 1000.0);


    // Basic lighting
    vec3 diffuse = vec3(0);
    if (!isInShadow) {
        diffuse = vec3(clamp(dot(toLightDir, hitResult.normal), 0, 1));
    }

    //specular
    float specularStrength=2.0;
    float specular=0;
    if (!isInShadow) {
        vec3 viewDir = gl_WorldRayDirectionEXT;
        vec3 reflectDir = reflect(toLightDir, hitResult.normal);
        specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    }
    vec3 specularColor = specularStrength * vec3(specular);

    vec3 light =vec3(AMBIENT_LIGHT_INTENSITY) + specularColor + diffuse;
    //occlusion
    float occlusionStrength=0.7;
    float occlusion = 0;
    //0.5 ray per pixel
   // if ((gl_LaunchIDEXT.x+gl_LaunchIDEXT.y)%2==0)
    {
        const int numSamples = 4;
        for (int i = 0; i < numSamples; i++)
        {
            vec2 coord = vec2(gl_LaunchIDEXT.xy) / vec2(gl_LaunchSizeEXT.xy);
            vec3 pos = vec3(coord.x, coord.y, time.value + (i * PHI));
            bool o = occlusionRay(origin, RandDirHemisphere(hitResult.normal, pos), 0.7);
            occlusion += float(o) / float(numSamples);
        }
    }
    light *=1-(occlusion*occlusionStrength);
    primaryRayPayload.color = material_color;
    primaryRayPayload.color *= light;
}
