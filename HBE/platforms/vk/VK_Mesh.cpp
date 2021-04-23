
#include "VK_Mesh.h"
#include "VK_Device.h"

namespace HBE {
    VK_Mesh::VK_Mesh(const HBE::VK_Device *device) {
        this->device=device;
    }

    void VK_Mesh::setIndices(const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) {

    }

    void VK_Mesh::bind() const {

    }

    void VK_Mesh::unbind() const {

    }
}



