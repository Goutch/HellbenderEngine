#version 330 core
out vec4 FragColor;
in vec2 uvs;
uniform vec2 resolution;
uniform sampler2D texture_0;
uniform float time;
const float exposure=1.;
const float gamma=1.5;
const bool grain_enabled=true;
vec3 noise()
{
    float strength = 16.0;
    float x = (uvs.x + 4.0) * (uvs.y + 4.0) * (time * 10.0);
    return vec3(mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01)-0.005) * strength;
}
void main()
{
    vec4 color=texture(texture_0, uvs);
    if (grain_enabled)
    {
        color.rgb= color.rgb*(noise()+0.5);
    }
    FragColor = vec4(pow(vec3(1.0) - exp(-color.rgb * exposure), vec3(1.0 / gamma)), color.a);
}