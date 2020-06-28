#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform bool has_texture;
uniform sampler2D texture_0;
uniform vec4 material_color;
void main()
{

    vec3 c;
    if(has_texture)
    {
        FragColor=texture(texture_0,uv)*material_color;
    }
    else
    {
        FragColor =material_color;
    }

}