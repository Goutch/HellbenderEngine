#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : require
#define SCALE 1000.0
#include "common.glsl"

layout(location = 0) rayPayloadInEXT PrimaryRayPayLoad
{
    vec3 color;
    int bounce_count;
    uint rng_state;
    bool hit_sky;
    vec3 hit_normal;
    float hit_t;
} primaryRayPayload;

const vec3 AMBIENT = vec3(0.1, 0.1, 0.1);
const int SAMPLE_COUNT=8;
const float START_ALTITUDE_RATIO = 0.125;
const float ATMOSPHERE_RADIUS = 12.0*SCALE;
const float PLANET_RADIUS =10.0*SCALE;
const vec3 WAVELENGTHS = vec3(
0.106,
0.324,
0.68);

float atmosphereDensity(vec3 point, vec3 planet_position)
{
    float altitude=length(point-planet_position) - PLANET_RADIUS;
    float altitude01= altitude/(ATMOSPHERE_RADIUS-PLANET_RADIUS);
    float density=exp(-altitude01*frame.density_falloff)*(1-altitude01);
    return density;
}

float averangeDensity(vec3 point, vec3 dir, float atmosphere_end_t, vec3 planet_position)
{
    vec3 sample_point = point;
    float step_size=atmosphere_end_t/float(SAMPLE_COUNT-1);
    float averange=0.0;
    for (int i=0; i<SAMPLE_COUNT; i++)
    {
        float density=atmosphereDensity(sample_point, planet_position);
        averange+=density*step_size/SCALE;
        sample_point+=dir*step_size;
    }
    return averange;
}
// Returns vector (dstToSphere, dstThroughSphere)
// If ray origin is inside sphere, dstToSphere = 0
// If ray misses sphere, dstToSphere = maxValue; dstThroughSphere = 0
vec2 raySphere(vec3 sphereCentre, float sphereRadius, vec3 rayOrigin, vec3 rayDir) {
    vec3 offset = rayOrigin - sphereCentre;
    float a = 1;// Set to dot(rayDir, rayDir) if rayDir might not be normalized
    float b = 2 * dot(offset, rayDir);
    float c = dot (offset, offset) - sphereRadius * sphereRadius;
    float d = b * b - 4 * a * c;// Discriminant from quadratic formula

    // Number of intersections: 0 when d < 0; 1 when d = 0; 2 when d > 0
    if (d > 0) {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        // Ignore intersections that occur behind the ray
        if (dstToSphereFar >= 0) {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }
    // Ray did not intersect sphere
    return vec2(sphereRadius*999, 0);
}
void main()
{
    vec3 toLightDir = normalize(vec3(sin(frame.time), sin(frame.time), cos(frame.time)));
    float sun_emision = max(pow(dot(toLightDir, gl_WorldRayDirectionEXT), E*20), 0);

    float atmosphere_height = ATMOSPHERE_RADIUS-PLANET_RADIUS;
    vec3 wavelength = WAVELENGTHS*frame.scattering_multiplier;
    vec3 origin =vec3(0, PLANET_RADIUS+(START_ALTITUDE_RATIO*(ATMOSPHERE_RADIUS-PLANET_RADIUS)), 0)+gl_WorldRayOriginEXT;
    vec3 planet_center =gl_WorldRayOriginEXT;
    planet_center.y=0;


    vec2 atmosphere_intersection = raySphere(planet_center, ATMOSPHERE_RADIUS, origin, gl_WorldRayDirectionEXT);
    float atmosphere_end_t = atmosphere_intersection.y-0.001;
    float atmosphere_start_t = atmosphere_intersection.x+0.001;
    float planet_start_t = raySphere(planet_center, PLANET_RADIUS, origin, gl_WorldRayDirectionEXT).x+0.001;
    origin += atmosphere_start_t*gl_WorldRayDirectionEXT;
    float atmosphere_travel_t= min(planet_start_t, atmosphere_end_t)-atmosphere_start_t;
    //float angle = asin(dot(toLightDir, gl_WorldRayDirectionEXT)/(length(toLightDir)*length(gl_WorldRayDirectionEXT)));
    float weight = 1.0;//(3.0*(1.0+pow(angle, 2.0)))/(16.0*PI);

    vec3 sunColor =vec3(sun_emision, sun_emision*0.6, sun_emision*0.4)*2;

    if (atmosphere_travel_t>0)
    {
        vec3 scatter_point = origin;
        float step_size=atmosphere_travel_t/float(SAMPLE_COUNT-1);
        vec3 scattered_light=vec3(0, 0, 0);
        for (int i=0; i<SAMPLE_COUNT; i++)
        {
            vec2 to_light_atmosphere_intersection = raySphere(planet_center, ATMOSPHERE_RADIUS, scatter_point, toLightDir);
            float to_light_atmosphere_end_t = to_light_atmosphere_intersection.y - 0.1;
            float to_light_atmosphere_start_t = to_light_atmosphere_intersection.x + 0.1;
            float to_light_planet_t = raySphere(planet_center, PLANET_RADIUS, origin, gl_WorldRayDirectionEXT).x-0.001;

            float to_light_travel_distance =  min(to_light_planet_t, to_light_atmosphere_end_t)-to_light_atmosphere_start_t;
            float to_light_density = averangeDensity(scatter_point, toLightDir, to_light_travel_distance, planet_center);

            float view_to_atmosphere_density = averangeDensity(scatter_point, -gl_WorldRayDirectionEXT, step_size*float(i), planet_center);
            vec3 transmittance = exp(-(to_light_density+view_to_atmosphere_density) * wavelength);
            float local_density = atmosphereDensity(scatter_point, planet_center);

            scattered_light += (local_density*weight* step_size/SCALE)*transmittance *wavelength;
            scatter_point += gl_WorldRayDirectionEXT * step_size;
        }

        sunColor += (scattered_light);
    }
    mix(vec3(1,1,1),sunColor,(dot(toLightDir,gl_WorldRayDirectionEXT)+1)/2.0);

    primaryRayPayload.bounce_count++;
    primaryRayPayload.color =sunColor;
    primaryRayPayload.bounce_count--;
    primaryRayPayload.hit_sky = true;
    primaryRayPayload.hit_t = gl_RayTmaxEXT;
    primaryRayPayload.hit_normal = -gl_WorldRayDirectionEXT;
    /*  vec3 planet_center = gl_WorldRayOriginEXT;
      planet_center.y=0;

      vec3 origin =gl_WorldRayOriginEXT+vec3(0, PLANET_RADIUS, 0);

      float atmosphere_end_t = raySphere(planet_center, ATMOSPHERE_RADIUS,origin, gl_WorldRayDirectionEXT).y;
      vec3 atmosphere_end_position = gl_WorldRayOriginEXT+gl_WorldRayDirectionEXT*atmosphere_end_t;

      float to_light_t = raySphere(atmosphere_end_position, ATMOSPHERE_RADIUS, atmosphere_end_position, toLightDir).y;



      float ratio = (((atmosphere_end_position.y/2)+(gl_RayTmaxEXT/2))/gl_RayTmaxEXT);
      vec3 sky_color = vec3(1-ratio, 0.3, ratio * 0.5);


      primaryRayPayload.bounce_count++;
      primaryRayPayload.color = (vec3(to_light_t)/ATMOSPHERE_RADIUS*2.0f)/primaryRayPayload.bounce_count;
      primaryRayPayload.bounce_count--;
      primaryRayPayload.hit_sky = true;
      primaryRayPayload.hit_t = gl_RayTmaxEXT;
      primaryRayPayload.hit_normal = -gl_WorldRayDirectionEXT;*/
}