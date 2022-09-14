#version 460
#extension GL_EXT_ray_tracing : require

float intersection(vec3 o, vec3 d, vec3 aabb_min, vec3 aabb_max) {
    float t[10];
    t[1] = (aabb_min.x - o.x)/d.x;
    t[2] = (aabb_max.x - o.x)/d.x;
    t[3] = (aabb_min.y - o.y)/d.y;
    t[4] = (aabb_max.y - o.y)/d.y;
    t[5] = (aabb_min.z - o.z)/d.z;
    t[6] = (aabb_max.z - o.z)/d.z;
    t[7] = max(max(min(t[1], t[2]), min(t[3], t[4])), min(t[5], t[6]));
    t[8] = min(min(max(t[1], t[2]), max(t[3], t[4])), max(t[5], t[6]));
    t[9] = (t[8] < 0 || t[7] > t[8]) ?-1: t[7];
    return t[9];
}
void main()
{

    mat4x3 transform = gl_ObjectToWorldEXT;

    vec3 p = vec3(transform[3][0], transform[3][0], transform[3][2]);
    vec3 s = vec3(length(transform[0].xyz), length(transform[1].xyz), length(transform[2].xyz));

    float t = intersection(gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, p-s, p+s);
    if (t>0)
    {
        reportIntersectionEXT(t, 0);
    }
    else
    {
        reportIntersectionEXT(0, 0);
    }

}