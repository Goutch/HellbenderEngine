#version 460
#extension GL_EXT_ray_tracing : require
// this method is documented in raytracing gems book
vec2 gems_intersections(vec3 orig, vec3 dir, vec3 center, float radius)
{
    vec3 f = orig - center;
    float a = dot(dir, dir);
    float bi = dot(-f, dir);
    float c = dot(f, f) - radius * radius;
    vec3 s = f + (bi/a)*dir;
    float discr = radius * radius - dot(s, s);

    vec2 t = vec2(-1.0, -1.0);
    if (discr >= 0) {
        float q = bi + sign(bi) * sqrt(a*discr);
        float t1 = c / q;
        float t2 = q / a;
        t = vec2(t1, t2);
    }
    return t;
}

void main()
{

    vec3 orig = gl_WorldRayOriginEXT;
    vec3 dir = gl_WorldRayDirectionEXT;

    vec3 aabb_max = vec3(sph.aabb_maxx, sph.aabb_maxy, sph.aabb_maxz);
    vec3 aabb_min = vec3(sph.aabb_minx, sph.aabb_miny, sph.aabb_minz);
    vec3 center = (aabb_max + aabb_min) / vec3(2.0);
    float radius = (aabb_max.x - aabb_min.x) / 2.0;

    vec2 t = gems_intersections(orig, dir, center, radius);

    sphere_point =  orig + t.x * dir;
    reportIntersectionEXT(t.x, 0);
    sphere_point =  orig + t.y * dir;
    reportIntersectionEXT(t.y, 0);
}