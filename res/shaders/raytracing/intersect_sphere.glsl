#version 460
#extension GL_EXT_ray_tracing : require

hitAttributeEXT HitResult
{
    vec3 normal;
} hitResult;

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
    mat4x3 transform = gl_ObjectToWorldEXT;

    vec3 p = vec3(transform[3][0], transform[3][1], transform[3][2]);
    vec3 s = vec3(length(transform[0].xyz), length(transform[1].xyz), length(transform[2].xyz));
    vec3 half_s = s * 0.5;

    vec2 ts = gems_intersections(gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, p, half_s.x);

    if (ts.x > 0.0)
    {
        vec3 hitPosition =gl_WorldRayOriginEXT + (ts.x*gl_WorldRayDirectionEXT);
        hitResult.normal = normalize(hitPosition-p);
        reportIntersectionEXT(ts.x, 0);
    }
    if (ts.y > 0.0)
    {
        vec3 hitPosition =gl_WorldRayOriginEXT + (ts.y*gl_WorldRayDirectionEXT);
        hitResult.normal = normalize(hitPosition-p);
        reportIntersectionEXT(ts.y, 0);
    }
}