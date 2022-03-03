#version 450
#extension GL_GOOGLE_include_directive : require

#include "../../res/shaders/pathTracer/Intersection.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 2) in vec3 worldPosition;
layout(location = 3) in flat mat4 fragInstanceTransform;
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
} ubo;
void main() {
    vec3 camera_position=inverse(ubo.view)[3].xyz;
    vec3 cube_position=fragInstanceTransform[3].xyz;
    Ray ray=Ray(camera_position, normalize(worldPosition-camera_position));
    Cube cube=Cube(cube_position-vec3(0.5, 0.5, 0.5), cube_position + vec3(0.5, 0.5, 0.5));
    Intersection intersection = intersectCube(ray, cube);
    float depth=intersection.t/100;
    gl_FragDepth=depth;

    outColor=vec4(1-depth, 1-depth, 1-depth, 1.);
}