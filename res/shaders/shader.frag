#version 330 core
out vec4 FragColor;
in vec2 uvs;

void main()
{
    vec4 color=vec4(uvs.x,0.2,uvs.y, 1.0f);
    FragColor = color;
}