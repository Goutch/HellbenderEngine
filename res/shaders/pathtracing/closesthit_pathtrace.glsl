#version 460
#extension GL_EXT_ray_tracing: enable
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_GOOGLE_include_directive : require
#include "common.glsl"
//-------------------------------CONSTANTS-------------------------------------

struct MaterialData
{
    vec4 color;
    float roughness;
};

//------------------------------------ UNIFORMS ------------------------------------
layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;



//layout(binding = 4, set = 0) readonly buffer MaterialDataBuffer
//{
//    MaterialData materials[];
//} materials;


// -------------------------- RAY PAYLOADS --------------------------------
layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int depth;
    uint rng_state;
    bool hit_sky;

} primaryRayPayload;


hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

struct Material {
    vec3 albedo;
    vec3 emissive;
    float roughness;
};

const Material materials[]= Material[7](
Material(vec3(0.8, 0.8, 0.8), vec3(0.0, 0.0, 0.0), 1.0),
Material(vec3(0.5, 0.5, 0.5), vec3(0.0, 0.0, 0.0), 1.0),
Material(vec3(0.5, 0.1, 0.1), vec3(0.0, 0.0, 0.0), 1.0),
Material(vec3(0.1, 0.5, 0.1), vec3(0.0, 0.0, 0.0), 1.0),
Material(vec3(0.1, 0.1, 0.5), vec3(0.0, 0.0, 0.0), 1.0),
Material(vec3(0.2, 0.2, 0.2), vec3(0.0, 0.0, 0.0), 0.4),
Material(vec3(0.8, 0.8, 0.8), vec3(0.0, 0.0, 0.0), 0.0)
);

//------------------------------------ FUNCTIONS ------------------------------------

uint wang_hash(inout uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float RandomFloat01(inout uint state)
{
    return float(wang_hash(state)) / 4294967296.0;
}

vec3 RandomUnitVector(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * TWO_PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}
vec3 RandomDiffuseVector(vec3 normal)
{
    return normalize(RandomUnitVector(primaryRayPayload.rng_state)+normal);
}
vec3 RandomSpecularVector(vec3 direction, vec3 diffuse, float roughness)
{
    return normalize(mix(direction, diffuse, roughness*roughness));
}

void traceLight(vec3 origin, vec3 dir, float tmax)
{
    const float tmin = 0.0001;
    traceRayEXT(topLevelAS, //topLevelacceleationStructure
    gl_RayFlagsOpaqueEXT|gl_RayFlagsSkipClosestHitShaderEXT|  gl_RayFlagsTerminateOnFirstHitEXT, //rayFlags
    0xFF, //cullMask
    0, //sbtRecordOffset
    1, //sbtRecordStride
    0, //missIndex
    origin, //origin
    tmin, //Tmin
    dir, //direction
    tmax, //Tmax
    0);//payload location
}
void trace(vec3 origin, vec3 dir, float tmax)
{
    const float tmin = 0.0001;
    primaryRayPayload.color=vec3(0.0);
    traceRayEXT(topLevelAS, //topLevelacceleationStructure
    gl_RayFlagsOpaqueEXT, //rayFlags
    0xFF, //cullMask
    0, //sbtRecordOffset
    1, //sbtRecordStride
    0, //missIndex
    origin, //origin
    tmin, //Tmin
    dir, //direction
    tmax, //Tmax
    0);//payload location
}

const float LIGHT_BIAS = 0.3;
void main()
{
    vec3 normal = hitResult.normal;

    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

    primaryRayPayload.depth++;
    uint numSamples=1;
    if (frame.sample_count<1)
    {
        numSamples= uint(mod(gl_LaunchIDEXT.x+gl_LaunchIDEXT.y+mod(frame.index, 2), 2));
    }
    else
    {
        numSamples = primaryRayPayload.depth==1?frame.sample_count:1;
    }
    Material material  = materials[gl_InstanceID%7];

    vec3 to_light_dir= normalize(vec3(sin(frame.time), sin(frame.time), cos(frame.time)));
    float light_dot_product=dot(hitResult.normal, to_light_dir);
    vec3 color =vec3(0.0);
    int num_sun_samples = 0;
    if (primaryRayPayload.depth<frame.max_bounces)
    {
        for (uint i = 0; i < numSamples; i++)
        {
            vec3 newDir;
            if (smoothstep(0, 1, light_dot_product)>RandomFloat01(primaryRayPayload.rng_state))
            {
                primaryRayPayload.hit_sky=false;
                primaryRayPayload.color = vec3(0.0);
                newDir = RandomSpecularVector(to_light_dir, RandomDiffuseVector(to_light_dir), LIGHT_BIAS);
                traceLight(origin, newDir, 1000.0);;

                if (primaryRayPayload.hit_sky && material.roughness>0.999)
                {
                    color += primaryRayPayload.color;
                    num_sun_samples++;
                }
            }

            primaryRayPayload.hit_sky=false;
            newDir = RandomDiffuseVector(hitResult.normal);
            if (material.roughness<0.999)
            {
                newDir = RandomSpecularVector(reflect(gl_WorldRayDirectionEXT, hitResult.normal), newDir, material.roughness);
            }
            trace(origin, newDir, 1000.0);
            color += primaryRayPayload.color;


            color += primaryRayPayload.color;
        }
        color /= float(numSamples+num_sun_samples);
    }

    primaryRayPayload.color = (material.emissive) + (material.albedo*color);
    primaryRayPayload.depth = primaryRayPayload.depth-1;
}

