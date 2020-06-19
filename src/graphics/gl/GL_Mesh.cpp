//
// Created by User on 31-May-2020.
//

#include "GL_Mesh.h"
#include "glad/glad.h"
GL_Mesh::GL_Mesh() {
    glGenVertexArrays(1, &vao);
}
GL_Mesh::~GL_Mesh() {
    for (auto buffer:buffers) {
        glDeleteBuffers(1,&buffer.second);
    }
    glDeleteBuffers(1,&ebo);
    glDeleteVertexArrays(1,&vao);
}
void GL_Mesh::setIndices(const std::vector<unsigned int> &data) {
    bind();
    index_count=data.size();
    if (!has_index_buffer) {
        has_index_buffer = true;
        glGenBuffers(1, &ebo);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size()*sizeof(unsigned int), data.data(), GL_STATIC_DRAW);
    unbind();
}

void GL_Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {
    setBuffer(position,data.data(),data.size(),sizeof(float),1);
}

void GL_Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {
    setBuffer(position,&data[0].x,data.size()*2,sizeof(float),2);
}

void GL_Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {
    setBuffer(position,&data[0].x,data.size()*3,sizeof(float),3);
}

void GL_Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {
    setBuffer(position,&data[0].x,data.size()*4,sizeof(float),4);
}

void GL_Mesh::bind() const{
    glBindVertexArray(vao);
}

void GL_Mesh::unbind() const{
    glBindVertexArray(0);
}

void GL_Mesh::setBuffer(unsigned int position,
                        const float *data,
                        unsigned int data_count,
                        const unsigned int byte_size,
                        unsigned int count_per_vertex) {
    bind();
    vertex_count=data_count;
    unsigned int vbo;
    auto vbo_it = buffers.find(position);
    if (vbo_it == buffers.end()) {
        glGenBuffers(1, &vbo);
        buffers.emplace(position, vbo);
    } else{
        vbo=vbo_it->second;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data_count*byte_size, data, GL_STATIC_DRAW);
    glVertexAttribPointer(position, count_per_vertex, GL_FLOAT, GL_FALSE, count_per_vertex*byte_size, nullptr);
    glEnableVertexAttribArray(position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbind();
}






