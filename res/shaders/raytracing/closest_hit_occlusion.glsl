#version 460
#extension GL_EXT_ray_tracing: require
#extension GL_EXT_nonuniform_qualifier: enable

//Cant use gl_RayTmaxEXT to know the max T at this stage see documentation:
/*
    The variables gl_RayTminEXT and gl_RayTmaxEXT are available in the intersection,
    any-hit, closest-hit, and miss shaders to specify the parametric <Tmin>
    and <Tmax> values of the ray being processed. The values are independent
    of the space in which the ray and origin exist.

    The <Tmin> value remains constant for the duration of the ray query, while
    the <Tmax> value changes throughout the lifetime of the ray query that
    produced the intersection. In the closest-hit shader, the value reflects
    the closest distance to the intersected primitive. In the any-hit shader,
    it reflects the distance to the primitive currently being intersected. In
    the intersection shader, it reflects the distance to the closest primitive
    intersected so far. The value can change in the intersection shader after
    calling reportIntersectionEXT() if the corresponding any-hit shader does not
    ignore the intersection. In a miss shader, the value is identical to the
    parameter passed into traceRayEXT().
*/
layout(location = 1) rayPayloadInEXT OcclusionRayPayload
{
    float occlusion;
    float MAX_T;
} occlusionRayPayload;

void main()
{
    occlusionRayPayload.occlusion = 1.0 - (gl_HitTEXT/occlusionRayPayload.MAX_T);
}