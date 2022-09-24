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
layout (binding = 4, set = 0) uniform Frame
{
    float time;
    uint index;
} frame;
//layout(binding = 4, set = 0) readonly buffer MaterialDataBuffer
//{
//    MaterialData materials[];
//} materials;


// -------------------------- RAY PAYLOADS --------------------------------
layout (location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
} primaryRayPayload;

layout(location = 1) rayPayloadEXT OcclusionRayPayload
{
    float occlusion;
    float MAX_T;
} occlusionRayPayload;

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

vec3 atmosphere(vec3 color, float t, float maxT)
{
    vec3 a = exp(-(4.6/maxT)*t*vec3(2+(sin(frame.time)*1.5), 2, 2+cos(frame.time)));
    return (color*a) + ((1.0-a) * vec3(0.5, 0.5, 0.5));
}
float occlusionRay(vec3 origin, vec3 dir, float tmax)
{
    const float tmin = 0.0001;
    occlusionRayPayload.MAX_T = tmax;
    occlusionRayPayload.occlusion = 0.0;
    // Trace shadow ray and offset indices to match shadow hit/miss shader group indices
    traceRayEXT(topLevelAS, //topLevelacceleationStructure
    gl_RayFlagsOpaqueEXT, // | gl_RayFlagsSkipClosestHitShaderEXT|  gl_RayFlagsTerminateOnFirstHitEXT, //rayFlags
    0xFF, //cullMask
    2, //sbtRecordOffset
    1, //sbtRecordStride
    1, //missIndex
    origin, //origin
    tmin, //Tmin
    dir, //direction
    tmax, //Tmax
    1);//payload location
    return occlusionRayPayload.occlusion;
}

void main()
{
    vec3 material_color = vec3(0.5);
    switch (gl_InstanceID%5)
    {
        case 0:material_color=vec3(0.5, 0.5, 0.5); break;
        case 1:material_color=vec3(1.0, 0.0, 0.0); break;
        case 2:material_color=vec3(0.0, 0.0, 1.0); break;
        case 3:material_color=vec3(0.0, 1.0, 0.0); break;
        case 4:material_color=vec3(1.0, 1.0, 1.0); break;
    }
    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

    vec3 toLightDir = normalize(vec3(sin(frame.time), sin(frame.time), cos(frame.time)));
    bool isInShadow = false;
    //if ((gl_LaunchIDEXT.x+gl_LaunchIDEXT.y)%2==0)
    isInShadow = occlusionRay(origin, toLightDir, 1000.0)>=0.01;


    // Basic lighting
    vec3 diffuse = vec3(0);
    if (!isInShadow) {
        diffuse = vec3(clamp(dot(toLightDir, hitResult.normal), 0, 1));
    }

    //specular
    float specularStrength=0.7;
    float specularSpread =4;
    float specular=0;
    if (!isInShadow) {
        vec3 viewDir = gl_WorldRayDirectionEXT;
        vec3 reflectDir = reflect(toLightDir, hitResult.normal);
        specular = pow(max(dot(viewDir, reflectDir), 0.0), specularSpread);
    }
    vec3 specularColor = specularStrength * vec3(specular);

    vec3 light =vec3(AMBIENT_LIGHT_INTENSITY) + specularColor + diffuse;
    //occlusion
    float occlusionStrength=1.0f;
    float occlusion = 0.0;
    const int numSamples =4;
    //0.5 ray per pixel
    if ((gl_LaunchIDEXT.x+gl_LaunchIDEXT.y)%2==0)
    {

        for (int i = 0; i < numSamples; i++)
        {
            vec2 coord = vec2(gl_LaunchIDEXT.xy) / vec2(gl_LaunchSizeEXT.xy);
            vec3 pos = vec3(coord.x, coord.y, frame.time + (i * PHI));
            float o = occlusionRay(origin, RandDirHemisphere(hitResult.normal, pos), 0.7);
            //if (o>0.01)
            // occlusion += 1;
            occlusion += o;
        }
    }
    light *= 1-((occlusion / float(numSamples))*occlusionStrength);
    primaryRayPayload.color = material_color;
    primaryRayPayload.color *= light;
    primaryRayPayload.color = atmosphere(primaryRayPayload.color, gl_HitTEXT, 5000.0);
}
