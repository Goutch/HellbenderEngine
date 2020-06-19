#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uvs;

out vec2 uvs;
uniform mat4 projection_matrix;

void main()
{
    uvs=vertex_uvs;
    gl_Position = projection_matrix*vec4(vertex_position, 1.0);
}