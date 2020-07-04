#pragma once

#include <core/resource/Mesh.h>
#include "unordered_map"

class GL_Mesh : public Mesh {
    unsigned int vao;
    unsigned int ebo;
    std::unordered_map<unsigned int, unsigned int> buffers;
public:
    GL_Mesh();

    void setBuffer(unsigned int position, const std::vector<int> &data) override;

    ~GL_Mesh();

    void bind() const override;

    void unbind() const override;

    void setIndices(const std::vector<unsigned int> &data) override;

    void setBuffer(unsigned int position, const std::vector<unsigned int> &data) override;

    void setBuffer(unsigned int position, const std::vector<float> &data) override;

    void setBuffer(unsigned int position, const std::vector<vec2> &data) override;

    void setBuffer(unsigned int position, const std::vector<vec3> &data) override;

    void setBuffer(unsigned int position, const std::vector<vec4> &data) override;


    void setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) override;

    void setBuffer(unsigned int position, const float *data, unsigned int data_count, unsigned int byte_size,
                   unsigned int count_per_vertex);

    unsigned int getVBO(unsigned int position);


};
