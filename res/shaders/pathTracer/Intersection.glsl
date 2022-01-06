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

const float FLOAT_MAX=10000000.0f;
Intersection[2] intersectCube(Ray ray, Cube cube)
{
    vec3 ts0=vec3((cube.min-ray.origin)/ray.direction);
    vec3 ts1=vec3((cube.max-ray.origin)/ray.direction);

    if (ts0.x<0.)ts0.x=FLOAT_MAX;
    if (ts0.y<0.)ts0.y=FLOAT_MAX;
    if (ts0.z<0.)ts0.z=FLOAT_MAX;
    if (ts1.x<0.)ts0.x=FLOAT_MAX;
    if (ts1.y<0.)ts0.y=FLOAT_MAX;
    if (ts1.z<0.)ts0.z=FLOAT_MAX;

    uint t0_index=ts0.x<ts0.y?(ts0.x<ts0.z?0:2):(ts0.y<ts0.z?1:2);
    uint t1_index=ts1.x<ts1.y?(ts1.x<ts1.z?0:2):(ts1.y<ts1.z?1:2);
    vec3 normal0=vec3(0., 0., 0.);
    vec3 normal1=vec3(0., 0., 0.);
    float t0=ts0[t0_index];
    float t1=ts1[t1_index];

    normal0[t0_index]=1.;
    normal1[t1_index]=1.;
    Intersection intersections[2];

    if (t0>t1)
    {
        intersections[0]=Intersection(t0, normal0);
        intersections[1]=Intersection(t1, normal1);
    }
    else
    {
        intersections[1]=Intersection(t0, normal0);
        intersections[0]=Intersection(t1, normal1);
    }
    return intersections;
}