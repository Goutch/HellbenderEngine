#version 450
#extension GL_GOOGLE_include_directive : require

#include "../../res/shaders/pathTracer/Intersection.glsl"

layout(location = 0) out vec4 outColor;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in flat mat4 fragInstanceTransform;


layout(binding = 0, set=0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;


layout(binding = 1, set=0, r8ui) uniform readonly uimage3D voxels;

layout(set =0, binding = 2) uniform VoxelDataUBO {
    uvec4 dimensions;
} VoxelData;



ivec3 voxel_pos;
float t=0.0f;

bool march(Ray ray)
{

    return true;
}

void main() {
    vec3 camera_position=inverse(ubo.view)[3].xyz;
    vec3 cube_position=fragInstanceTransform[3].xyz;
    Ray ray=Ray(camera_position, normalize(worldPosition-camera_position));
    Cube cube=Cube(cube_position-vec3(0.5, 0.5, 0.5), cube_position + vec3(0.5, 0.5, 0.5));
    Intersection intersection = intersectCube(ray, cube);

    vec3 position = (ray.origin+(ray.direction*intersection.t));
    vec3 fdimensions= vec3(VoxelData.dimensions.xyz);
    voxel_pos=clamp(ivec3(floor((position-cube_position+vec3(0.5, 0.5, 0.5))*fdimensions)), ivec3(0, 0, 0), ivec3(VoxelData.dimensions.xyz)-ivec3(1, 1, 1));

    t=intersection.t;



    ivec3 step = ivec3(ray.direction.x < 0 ? -1 : 1, ray.direction.y < 0 ? -1 : 1, ray.direction.z < 0 ? -1 : 1);
    float voxel_size=1/fdimensions.x;
    /* vec3 planes = (position-(mod(position, voxel_size)))+vec3(
     (int(step.x > 0?1:0)) * voxel_size,
     (int(step.y > 0?1:0)) * voxel_size,
     (int(step.z > 0?1:0)) * voxel_size);*/

    vec3 planes = vec3(
    step.x > 0 ? ceil(voxel_pos.x + 0.5) * voxel_size : floor(voxel_pos.x + 0.5) * voxel_size,
    step.y > 0 ? ceil(voxel_pos.y + 0.5) * voxel_size : floor(voxel_pos.y + 0.5) * voxel_size,
    step.z > 0 ? ceil(voxel_pos.z + 0.5) * voxel_size : floor(voxel_pos.z + 0.5) * voxel_size)+cube_position-vec3(0.5, 0.5, 0.5);
    uint v=imageLoad(voxels, voxel_pos).r;

    bool sucess =true;
    while (v==0)
    {

        vec3 ts = (planes - ray.origin) / ray.direction;
        int step_index=0;
        t = ts.x;

        if (ts.y < t)
        {
            t = ts.y;
            step_index = 1;
        }
        if (ts.z < t)
        {
            t=ts.z;
            step_index = 2;
        }
        voxel_pos[step_index]+=step[step_index];

        if (voxel_pos[step_index]<0||voxel_pos[step_index]>=VoxelData.dimensions[step_index])
        {
            sucess=false;
            break;
        }
        planes[step_index]+=step[step_index]*voxel_size;
        v=imageLoad(voxels, voxel_pos).r;
    }

    if (sucess)
    {
        float depth=t/100;
        vec3 color= vec3(voxel_pos)/vec3(fdimensions);
        gl_FragDepth=depth;
        outColor=vec4((1-depth), (1-depth), (1-depth), 1.)*vec4(color, 1.0f);
    }
    else
    {
        discard;
    }
}