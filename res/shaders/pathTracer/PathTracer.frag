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
    uvec3 resolution;
    vec3 bounding_box_size;
} VoxelData;

void main() {
    vec3 half_size=(VoxelData.bounding_box_size/2.0f);
    vec3 voxel_size=VoxelData.bounding_box_size/vec3(VoxelData.resolution);
    vec3 camera_position=inverse(ubo.view)[3].xyz;
    vec3 cube_position=fragInstanceTransform[3].xyz;
    Ray ray=Ray(camera_position, normalize(worldPosition-camera_position));
    ivec3 step = ivec3(ray.direction.x < 0 ? -1 : 1, ray.direction.y < 0 ? -1 : 1, ray.direction.z < 0 ? -1 : 1);
    Cube cube=Cube(cube_position-half_size, cube_position + half_size);

    Intersection intersection = intersectCube(ray, cube);
    float t=intersection.t;

    vec3 position = (ray.origin+(ray.direction*intersection.t));
    ivec3 voxel_pos=clamp(ivec3(floor((((position-cube_position)+half_size)/VoxelData.bounding_box_size)*vec3(VoxelData.resolution))), ivec3(0, 0, 0), ivec3(VoxelData.resolution)-ivec3(1, 1, 1));

    vec3 planes =cube.min+ vec3(
    (voxel_pos.x+int(step.x > 0)) * voxel_size.x,
    (voxel_pos.y+int(step.y > 0)) * voxel_size.y,
    (voxel_pos.z+int(step.z > 0)) * voxel_size.z);

    bool sucess = true;
    int step_index = 0;

    uint v = imageLoad(voxels, voxel_pos).r;
    while (v == 0)
    {

        vec3 ts = (planes - ray.origin) / ray.direction;
        step_index=0;

        int x_lesser_than_y=int(ts.x < ts.y);
        int y_lesser_than_x=int(ts.y < ts.x);
        int xy_lesser_than_z = int(ts[y_lesser_than_x]< ts.z);
        int z_lesser_than_xy = int(ts.z<ts[y_lesser_than_x]);

        step_index=y_lesser_than_x+z_lesser_than_xy+(x_lesser_than_y *z_lesser_than_xy);
        t=ts[step_index];

        voxel_pos[step_index] += step[step_index];

        if (voxel_pos[step_index]<0 || voxel_pos[step_index] >= VoxelData.resolution[step_index])
        {
            sucess=false;
            break;
        }
        planes[step_index] += step[step_index]*voxel_size[step_index];
        v=imageLoad(voxels, voxel_pos).r;
    }

    if (sucess)
    {
        float depth=t/100;
        vec3 color= vec3(voxel_pos)/vec3(VoxelData.resolution);
        gl_FragDepth=depth;
        outColor=vec4((1-depth), (1-depth), (1-depth), 1.)*vec4(color, 1.0f);
    }
    else
    {
        discard;
    }
}