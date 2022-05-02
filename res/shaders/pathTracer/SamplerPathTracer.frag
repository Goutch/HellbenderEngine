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
const int MIN_LOD = 0;

layout(binding = 1, set=0) uniform usampler3D voxels;
layout(set =0, binding = 5) uniform VoxelDataUBO {
    uvec3 resolution;
    vec3 bounding_box_size;
} VoxelData;


vec3 Atmosphere(float t)
{
    return exp(-0.005*t*vec3(4, 2, 1));
}

uint loadVoxel(int lod, vec3 coord)
{
    return textureLod(voxels, coord, lod).r;
}
void main() {
    vec3 aabb_size = VoxelData.bounding_box_size;
    vec3 half_aabb_size = aabb_size/2.0;
    vec3 aabb_position=fragInstanceTransform[3].xyz;
    Cube aabb=Cube(aabb_position-half_aabb_size, aabb_position + half_aabb_size);

    vec3 camera_position=inverse(ubo.view)[3].xyz;

    Ray ray=Ray(camera_position, normalize(worldPosition-camera_position));

    int lod=0;
    ivec3 iresolutions[LOD_COUNT];
    vec3 resolutions[LOD_COUNT];
    vec3 voxel_sizes[LOD_COUNT];
    vec3 half_sizes[LOD_COUNT];
    vec3 voxel_unit_sizes[LOD_COUNT];
    vec3 half_unit_sizes[LOD_COUNT];
    vec3 unit_steps[LOD_COUNT];
    ivec3 step_sign =ivec3(ray.direction.x < 0 ? -1 : 1, ray.direction.y < 0 ? -1 : 1, ray.direction.z < 0 ? -1 : 1);
    ivec3 step_positive = ivec3(int(step_sign.x > 0), int(step_sign.y > 0), int(step_sign.z > 0));
    for (int i =0;i<LOD_COUNT;i++)
    {
        iresolutions[i]=ivec3(VoxelData.resolution)/(1<<i);
        resolutions[i]=vec3(iresolutions[i]);
        voxel_sizes[i]=VoxelData.bounding_box_size/resolutions[i];
        half_sizes[i]=voxel_sizes[i]/2.0f;
        voxel_unit_sizes[i]=1.0f/resolutions[i];
        half_unit_sizes[i]=voxel_unit_sizes[i]/2.0f;

        unit_steps[i]= step_sign * voxel_unit_sizes[i];
    }

    Intersection intersection = intersectCube(ray, aabb);
    float t=intersection.t;

    vec3 position = (ray.origin+(ray.direction*t));

    vec3 position_object_space = (position-aabb_position);

    vec3 position_unit_voxel_space = (position_object_space+half_aabb_size) /aabb_size;

    //vec3 voxel_pos=clamp(floor(position_unit_voxel_space*resolutions[lod]), vec3(0., 0., 0.), resolutions[lod]-vec3(1., 1., 1.));

    vec3 voxel_unit_pos = ((position_unit_voxel_space-mod(position_unit_voxel_space, voxel_unit_sizes[lod])))+half_unit_sizes[lod];

    ivec3 voxel_position = ivec3(floor(voxel_unit_pos*resolutions[lod]));
    vec3 planes = aabb.min + (voxel_unit_pos * resolutions[lod] * voxel_sizes[lod]) + (step_sign*half_sizes[lod]);

    int step_axis_index = 0;

    uint v = loadVoxel(lod, voxel_unit_pos);
    vec3 ts = (planes - ray.origin) / ray.direction;
    int x_lesser_than_y=0;
    int y_lesser_than_x=0;
    int xy_lesser_than_z=0;
    int z_lesser_than_xy=0;
    while (true)
    {

        /*while (lod!=MIN_LOD&&v!=0)
        {
            vec3 voxel_world_position=(voxel_unit_pos*resolutions[lod]*voxel_sizes[lod])+aabb.min;

            lod--;
            voxel_unit_pos = ((voxel_unit_pos-(half_unit_sizes[lod]))) + vec3(
            voxel_unit_sizes[lod].x*int(position.x > voxel_world_position.x),
            voxel_unit_sizes[lod].y*int(position.y > voxel_world_position.y),
            voxel_unit_sizes[lod].z*int(position.z > voxel_world_position.z));

            planes = aabb.min + (voxel_unit_pos * resolutions[lod] * voxel_sizes[lod]) + (step_sign*half_sizes[lod]);

            v=loadVoxel(lod, voxel_unit_pos);
        }*/

        //found voxel
        if (v!=0)
        {
            break;
        }

        if (ts.x<ts.y&&ts.x<ts.z)
        {

            t=ts.x;
            voxel_unit_pos.x += unit_steps[lod].x;
            planes.x += step_sign.x * voxel_sizes[lod].x;
            if (voxel_unit_pos.x <0.0f || voxel_unit_pos.x >= 1.0f)
            {
                v=0;
                break;
            }
        }
        else if (ts.z<ts.x&&ts.z<ts.y)
        {
            t=ts.z;
            voxel_unit_pos.z += unit_steps[lod].z;
            planes.z += step_sign.z * voxel_sizes[lod].z;
            if (voxel_unit_pos.z <0.0f || voxel_unit_pos.z >= 1.0f)
            {
                v=0;
                break;
            }
        }
        else
        {
            t=ts.y;
            voxel_unit_pos.y += unit_steps[lod].y;
            planes.y += step_sign.y * voxel_sizes[lod].y;
            if (voxel_unit_pos.y <0.0f || voxel_unit_pos.y >= 1.0f)
            {
                v=0;
                break;
            }
        }

        ts = (planes - ray.origin) / ray.direction;
        position=(ray.origin+(ray.direction*t));

        v=loadVoxel(lod, voxel_unit_pos);
    }

    if (v!=0)
    {
        float depth=t/100;
        vec3 color= voxel_unit_pos;
        vec3 atmosphere =Atmosphere(t);
        //color=(atmosphere*color) + ((vec3(1)-atmosphere)*vec3(0.5, 0.5, 0.5));
        gl_FragDepth=depth;
        //outColor=vec4(1.-depth,1.-depth,1.-depth,1.0);

        outColor=vec4(color, 1.0f);
    }
    else
    {
        discard;
    }
}
