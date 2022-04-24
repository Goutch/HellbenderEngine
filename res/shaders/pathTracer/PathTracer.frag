#version 450

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Intersection{
    float t;
    vec3 normal;
};

struct Cube{
    vec3 min;
    vec3 max;
};

struct Sphere{
    vec3 position;
    float radius;
};

struct Plane{
    vec3 normal;
    float distance;
};

Intersection intersectCube(Ray ray, Cube cube)
{
    if (ray.origin.x>cube.min.x &&
    ray.origin.y>cube.min.y&&
    ray.origin.z>cube.min.z&&
    ray.origin.x<cube.max.x &&
    ray.origin.y<cube.max.y&&
    ray.origin.z<cube.max.z)
    {
        Intersection intersection;
        intersection.t = 0;
        intersection.normal = -ray.direction;

        return intersection;
    }
    vec3 min_ts=vec3((cube.min-ray.origin)/ray.direction);
    vec3 max_ts=vec3((cube.max-ray.origin)/ray.direction);

    vec3 ts0=min(min_ts, max_ts);

    vec3 ts1=max(min_ts, max_ts);

    uint t0_index=ts0.x>ts0.y?
    (ts0.x>ts0.z?0:2):
    (ts0.y>ts0.z?1:2);

    uint t1_index=ts1.x<ts1.y?
    (ts1.x<ts1.z?0:2):
    (ts1.y<ts1.z?1:2);

    vec3 t0_normal=vec3(0., 0., 0.);
    vec3 t1_normal=vec3(0., 0., 0.);

    float t0=ts0[t0_index];
    float t1=ts1[t1_index];

    t0_normal[t0_index]=(-ray.direction[t0_index]);
    // t0_normal=normalize(t0_normal);

    t1_normal[t1_index]=(ray.direction[t1_index]);
    //t1_normal=normalize(t1_normal);

    Intersection intersection;

    intersection.t = t0;
    intersection.normal = t0_normal;

    return intersection;
}

layout(location = 0) out vec4 outColor;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in flat mat4 fragInstanceTransform;


layout(binding = 0, set=0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;

const int LOD_COUNT=4;
layout(binding = 1, set=0, r8ui) uniform readonly uimage3D voxels0;
layout(binding = 2, set=0, r8ui) uniform readonly uimage3D voxels1;
layout(binding = 3, set=0, r8ui) uniform readonly uimage3D voxels2;
layout(binding = 4, set=0, r8ui) uniform readonly uimage3D voxels3;
layout(set =0, binding = 5) uniform VoxelDataUBO {
    uvec3 resolution;
    vec3 bounding_box_size;
} VoxelData;


vec3 Atmosphere(float t)
{
    return exp(-0.005*t*vec3(4, 2, 1));
}

uint loadVoxel(int lod, ivec3 coord)
{
    switch (lod)
    {
        case 0:
        return imageLoad(voxels0, coord).r;
        case 1:
        return imageLoad(voxels1, coord).r;
        case 2:
        return imageLoad(voxels2, coord).r;
        case 3:
        return imageLoad(voxels3, coord).r;
    }
    return 0;
}
void main() {
    vec3 half_size=(VoxelData.bounding_box_size/2.0f);
    vec3 camera_position=inverse(ubo.view)[3].xyz;
    vec3 cube_position=fragInstanceTransform[3].xyz;
    Ray ray=Ray(camera_position, normalize(worldPosition-camera_position));
    ivec3 step = ivec3(ray.direction.x < 0 ? -1 : 1, ray.direction.y < 0 ? -1 : 1, ray.direction.z < 0 ? -1 : 1);
    Cube cube=Cube(cube_position-half_size, cube_position + half_size);

    Intersection intersection = intersectCube(ray, cube);
    float t=intersection.t;

    int lod=2;

    ivec3 resolutions[LOD_COUNT];
    vec3 voxel_sizes[LOD_COUNT];


    for (int i =0;i<LOD_COUNT;i++)
    {
        resolutions[i]=ivec3(VoxelData.resolution)/ivec3(1<<i);
        voxel_sizes[i]=VoxelData.bounding_box_size/vec3(resolutions[i]);
    }

    vec3 position = (ray.origin+(ray.direction*intersection.t));
    ivec3 voxel_pos=clamp(ivec3(floor((((position-cube_position)+half_size)/VoxelData.bounding_box_size)*vec3(resolutions[lod]))), ivec3(0, 0, 0), resolutions[lod]-ivec3(1, 1, 1));

    vec3 planes =cube.min+ vec3(
    (voxel_pos.x+int(step.x > 0)) * voxel_sizes[lod].x,
    (voxel_pos.y+int(step.y > 0)) * voxel_sizes[lod].y,
    (voxel_pos.z+int(step.z > 0)) * voxel_sizes[lod].z);

    bool sucess = true;
    int step_index = 0;


    uint v = loadVoxel(lod, voxel_pos);
    while (true)
    {

        while (true)
        {
            if (lod==0||v==0)
            break;
            //should be position compared to voxel position
            //step = ivec3(a>b)

            vec3 voxel_world_position=(voxel_pos*voxel_sizes[lod])+(voxel_sizes[lod]/2.)+cube.min;

            lod--;
            voxel_pos = (voxel_pos*2) + ivec3(int(position.x > voxel_world_position.x),
            int(position.y > voxel_world_position.y),
            int(position.z > voxel_world_position.z));
            vec3 planes =cube.min + vec3(
            (voxel_pos.x+int(step.x > 0)) * voxel_sizes[lod].x,
            (voxel_pos.y+int(step.y > 0)) * voxel_sizes[lod].y,
            (voxel_pos.z+int(step.z > 0)) * voxel_sizes[lod].z);

            v=loadVoxel(lod, voxel_pos);
        }
        //found voxel
        if (v!=0)
        {
            sucess=true;
            break;
        }

        //just calculae new t for step_index
        vec3 ts = (planes - ray.origin) / ray.direction;
        step_index=0;

        int x_lesser_than_y=int(ts.x < ts.y);
        int y_lesser_than_x=int(ts.y < ts.x);
        int xy_lesser_than_z = int(ts[y_lesser_than_x]< ts.z);
        int z_lesser_than_xy = int(ts.z<ts[y_lesser_than_x]);

        step_index=y_lesser_than_x + z_lesser_than_xy+(x_lesser_than_y * z_lesser_than_xy);
        t=ts[step_index];

        voxel_pos[step_index] += step[step_index];
        position[step_index]=planes[step_index];
        planes[step_index] += step[step_index]*voxel_sizes[lod][step_index];

        //out of bound
        if (voxel_pos[step_index]<0 || voxel_pos[step_index] >= resolutions[lod][step_index])
        {
            sucess=false;
            break;
        }

        v=loadVoxel(lod, voxel_pos);
    }

    if (sucess)
    {
        float depth=t/100;
        vec3 color= vec3(voxel_pos)/vec3(resolutions[0]);
        vec3 atmosphere =Atmosphere(t);
        color=(atmosphere*color) + ((vec3(1)-atmosphere)*vec3(0.5, 0.5, 0.5));
        gl_FragDepth=depth;
        outColor=vec4(color, 1.0f);
    }
    else
    {
        discard;
    }
}
