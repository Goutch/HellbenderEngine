#version 460
#extension GL_EXT_ray_tracing: require

hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

vec4 intersectAABB(vec3 o, vec3 d, vec3 aabb_max, vec3 aabb_min)
{
    if (!(o.x<=aabb_min.x+gl_RayTminEXT||
    o.y<=aabb_min.y+gl_RayTminEXT||
    o.z<=aabb_min.z+gl_RayTminEXT||
    o.x>=aabb_max.x-gl_RayTminEXT||
    o.y>=aabb_max.y-gl_RayTminEXT||
    o.z>=aabb_max.z-gl_RayTminEXT))
    {
        return vec4(-d.x, -d.y, -d.z, gl_RayTminEXT+0.001);
    }
    vec3 inv_dir = 1.0 / d;
    vec3 aabb_min_ts = vec3((aabb_min - o) * inv_dir);
    vec3 aabb_max_ts = vec3((aabb_max - o) * inv_dir);

    vec3 ts_min = min(aabb_min_ts, aabb_max_ts);
    vec3 ts_max = max(aabb_min_ts, aabb_max_ts);

    uint t_near_index=ts_min.x>ts_min.y?
    (ts_min.x>ts_min.z?0:2):
    (ts_min.y>ts_min.z?1:2);
    uint t_far_index=ts_max.x<ts_max.y?
    (ts_max.x<ts_max.z?0:2):
    (ts_max.y<ts_max.z?1:2);

    float t_near = ts_min[t_near_index];
    float t_far = ts_max[t_far_index];

    vec3 normal =vec3(0.);
    if (t_far >= t_near)
    {
        if (t_near >= gl_RayTminEXT)
        {
            normal[t_near_index] = -d[t_near_index];
            normal = normalize(normal);
            return vec4(normal.xyz, t_near);
        }

        if (t_far >= gl_RayTminEXT)
        {
            normal[t_near_index] = d[t_near_index];
            normal = normalize(normal);
            return vec4(normal.xyz, t_far);
        }
    }

    return vec4(0., 0., 0., -1);
}
void main()
{
    mat4x3 transform = gl_ObjectToWorldEXT;
    vec3 p = vec3(transform[3][0], transform[3][1], transform[3][2]);
    vec3 s = vec3(length(transform[0].xyz), length(transform[1].xyz), length(transform[2].xyz));
    vec3 half_size = s * 0.5;
    vec4 result = intersectAABB(gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, p + half_size, p -half_size);
    float t = result.w;
    if (t > 0.0)
    {
        vec3 normal = result.xyz;
        vec3 hitPos = gl_WorldRayOriginEXT + (gl_WorldRayDirectionEXT*t);

        hitResult.normal = normalize(normal);
        reportIntersectionEXT(t, 0);
    }
}