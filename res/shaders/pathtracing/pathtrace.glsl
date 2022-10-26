layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 9, set = 0, std430) readonly buffer MaterialDataBuffer
{
    MaterialData materials[];
} materials;

layout(binding = 10, set = 0, rgba8ui) uniform readonly uimage2D blueNoise;

// -------------------------- RAY PAYLOADS --------------------------------
layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int bounce_count;
    uint noise_sample_count;
    uint rng_state;
    bool hit_sky;
    vec3 hit_normal;
    float hit_t;
} primaryRayPayload;

const float LIGHT_BIAS = 0.3;
vec3 sampleBlueNoise(ivec2 coord)
{
    vec4 noise = imageLoad(blueNoise, coord);
    return vec3(noise.rgb)/vec3(255.0);
}
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

vec3 RandomUnitVectorRNG(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * TWO_PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}
vec3 RandomUnitVectorNoise()
{
    ivec2 coord = ivec2(gl_LaunchIDEXT.xy);
    float index = float(frame.index+primaryRayPayload.noise_sample_count);
    vec2 offset = vec2(index*PHI,index*E);

    offset=mod(offset,1.0);
    offset *= 469.0;
    coord += ivec2(offset);
    coord = ivec2(mod(vec2(coord), vec2(469)));
    primaryRayPayload.noise_sample_count++;
    vec3 noise = sampleBlueNoise(coord);
    return normalize(noise-vec3(0.5));
}
vec3 RandomDiffuseVectorFromNoise(vec3 normal)
{
    return normalize(RandomUnitVectorNoise() + normal);
}
vec3 RandomDiffuseVectorFromRNG(vec3 normal)
{
    return normalize(RandomUnitVectorRNG(primaryRayPayload.rng_state)+normal);
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

void traceRays(vec3 normal)
{
    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

    primaryRayPayload.bounce_count++;
    uint numSamples=1;
    if (frame.sample_count<1)
    {
        numSamples = uint(mod(gl_LaunchIDEXT.x+gl_LaunchIDEXT.y+mod(frame.index, 2), 2));
    }
    else
    {
        numSamples = primaryRayPayload.bounce_count==1?frame.sample_count:1;
    }
    MaterialData material  = materials.materials[gl_InstanceCustomIndexEXT];

    vec3 to_light_dir= normalize(vec3(sin(frame.time), sin(frame.time), cos(frame.time)));
    float light_dot_product=dot(hitResult.normal, to_light_dir);
    vec3 color =vec3(0.0);
    int num_sun_samples = 0;
    if (primaryRayPayload.bounce_count<frame.max_bounces)
    {
        for (uint i = 0; i < numSamples; i++)
        {
            vec3 newDir;
            if (material.roughness>0.999 && smoothstep(0, 1, light_dot_product)>RandomFloat01(primaryRayPayload.rng_state))
            {

                primaryRayPayload.hit_sky=false;
                primaryRayPayload.color = vec3(0.0);
                vec3 diffuseVector;
                if (bool(frame.use_blue_noise))
                {
                    diffuseVector = RandomDiffuseVectorFromNoise(to_light_dir);
                }
                else
                {
                    diffuseVector = RandomDiffuseVectorFromRNG(to_light_dir);
                }
                newDir = RandomSpecularVector(to_light_dir, diffuseVector, LIGHT_BIAS);
                traceLight(origin, newDir, 1000.0);;

                if (primaryRayPayload.hit_sky)
                {
                    color += primaryRayPayload.color;//*dot(hitResult.normal, newDir);
                    num_sun_samples++;
                }
            }

            primaryRayPayload.hit_sky=false;

            if(bool(frame.use_blue_noise))
            {
                newDir = RandomDiffuseVectorFromNoise(hitResult.normal);
            }
            else
            {
                newDir = RandomDiffuseVectorFromRNG(hitResult.normal);
            }

            if (material.roughness<0.999)
            {
                newDir = RandomSpecularVector(reflect(gl_WorldRayDirectionEXT, hitResult.normal), newDir, material.roughness);
            }
            trace(origin, newDir, 1000.0);
            color += primaryRayPayload.color*dot(hitResult.normal, newDir);

        }
        color /= float(numSamples+num_sun_samples);
    }

    primaryRayPayload.color = (material.emission.rgb) + (color*material.albedo.rgb);
    primaryRayPayload.hit_normal = hitResult.normal;
    primaryRayPayload.hit_t = gl_HitTEXT;
    primaryRayPayload.bounce_count = primaryRayPayload.bounce_count-1;
}