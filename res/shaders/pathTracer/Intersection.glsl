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