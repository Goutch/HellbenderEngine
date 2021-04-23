#include "Mesh.h"
#include "IMesh.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Renderer.h"
#include "core/resource/IResourceFactory.h"

namespace HBE {

    Mesh::Mesh() {
        instance = Graphics::getRenderer()->getResourceFactory()->createMesh();
    }

    Mesh::~Mesh() {
        delete instance;
    }

    void Mesh::setIndices(const std::vector<unsigned int> &data) {
        has_index_buffer=true;
        index_count=data.size();
        instance->setIndices(data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<int> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setBuffer(unsigned int position, const std::vector<unsigned int> &data) {
        vertex_count=data.size();
        instance->setBuffer(position, data);
    }

    void Mesh::setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) {
        instance_count = data.size();
        instance->setInstancedBuffer(position, data);
    }

    unsigned int Mesh::getVertexCount() const {
        return vertex_count;
    }

    unsigned int Mesh::getIndexCount() const {
        return index_count;
    }

    unsigned int Mesh::getInstanceCount() const {
        return instance_count;
    }

    bool Mesh::hasIndexBuffer() const {
        return has_index_buffer;
    }

    void Mesh::bind() const {
        instance->bind();
    }

    void Mesh::unbind() const {
        instance->unbind();
    }
}
