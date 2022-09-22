#version 460
#extension GL_EXT_ray_tracing: enable
#extension GL_EXT_nonuniform_qualifier: enable
//-------------------------------CONSTANTS-------------------------------------
const float AMBIENT_LIGHT_INTENSITY = 0.15;
const float PHI = 1.61803398874989484820459;// Φ = Golden Ratio
struct MaterialData
{
    vec4 color;
    float roughness;
};


//------------------------------------ UNIFORMS ------------------------------------
layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout (binding = 3, set = 0) uniform TimeUBO
{
    float value;
} time;

//layout(binding = 4, set = 0) readonly buffer MaterialDataBuffer
//{
//    MaterialData materials[];
//} materials;


// -------------------------- RAY PAYLOADS --------------------------------
layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int depth;
    int sample_id;
    float hit_t;
} primaryRayPayload;


hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

//------------------------------------ FUNCTIONS ------------------------------------
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
void trace(vec3 origin, vec3 dir, float tmax)
{
    const float tmin = 0.0001;

    primaryRayPayload.depth++;
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


void main()
{

    primaryRayPayload.depth++;
    vec3 normal = hitResult.normal;
    const int MAX_DEPTH = 6;
    vec3 toLightDir = normalize(vec3(sin(time.value), sin(time.value), cos(time.value)));
    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    //send a new ray
    if (primaryRayPayload.depth<MAX_DEPTH)
    {
        vec2 coord = vec2(gl_LaunchIDEXT.xy) / vec2(gl_LaunchSizeEXT.xy);
        vec3 pos = vec3(coord.x, coord.y, time.value + (((MAX_DEPTH*primaryRayPayload.sample_id) + primaryRayPayload.depth) * PHI));
        vec3 newDir = RandDirHemisphere(hitResult.normal, pos);
        trace(origin, newDir, 1000.0);
    }

    vec3 material_color = vec3(0.5);
    switch (gl_InstanceID%5)
    {
        case 0:material_color=vec3(0.5, 0.5, 0.5); break;
        case 1:material_color=vec3(1.0, 0.0, 0.0); break;
        case 2:material_color=vec3(0.0, 0.0, 1.0); break;
        case 3:material_color=vec3(0.0, 1.0, 0.0); break;
        case 4:material_color=vec3(1.0, 1.0, 1.0); break;
    }

    primaryRayPayload.color += material_color * (1.0 / primaryRayPayload.depth);
    primaryRayPayload.hit_t = gl_HitTEXT;
}

