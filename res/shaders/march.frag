#version 330 core
out vec4 FragColor;
in vec2 uvs;
uniform camera_position;
uniform camera_direction;
uniform vec2 resolution;
const vec4 sphere=vec4(1.);
const vec4 plane=vec4(1.);
float SceneDistance(vec3 p)
{

    float sphereDist =  length(p-s.xyz)-s.w;
    float planeDist = p.y;
    float d=min(sphereDist,planeDist);
    return d;
}
vec4 March(vec3 ro,vec3 rd){
    float dist_origin;
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;

    for(int i=0; i<MAX_STEPS; i++) {
        vec3 current_point = ro + rd*dist_origin;
        float dist_scene = SceneDistance(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    return vec4(col.xyz,dist_origin);
}
void main()
{
    vec2 uv = (uvs-.5*resolution.xy)/resolution.y;
    vec4 march_result=March(camera_position,camera_direction);
    float distance=march_result.w;
    vec3 color=march_result.rgb;

    color = pow(color, vec3(.4545));	// gamma correction

    FragColor = vec4(color,1.);
}