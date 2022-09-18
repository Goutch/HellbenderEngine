#version 460
#extension GL_EXT_ray_tracing : require

hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

struct Intersection{
    float t;
    vec3 normal;
};

Intersection intersectAABB(vec3 o, vec3 d, vec3 aabb_max, vec3 aabb_min)
{
    vec3 min_ts=vec3((aabb_min-o)/d);
    vec3 max_ts=vec3((aabb_max-o)/d);

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

    t0_normal[t0_index]=(-d[t0_index]);
    t0_normal=normalize(t0_normal);

    t1_normal[t1_index]=(d[t1_index]);
    t1_normal=normalize(t1_normal);

    Intersection intersection;

    if (t0>0)
    {
        intersection.t = t0;
        intersection.normal = t0_normal;
    }
    else
    {
        intersection.t = t1;
        intersection.normal = t1_normal;
    }

    return intersection;
}
void main()
{
    mat4x3 transform = gl_ObjectToWorldEXT;
    vec3 p = vec3(transform[3][0], transform[3][1], transform[3][2]);
    vec3 s = vec3(length(transform[0].xyz), length(transform[1].xyz), length(transform[2].xyz));
    vec3 half_s = s * 0.5;
    Intersection intersection = intersectAABB(gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, p - half_s, p + half_s);

    hitResult.normal = intersection.normal;

    reportIntersectionEXT(intersection.t, 0);
}